// SPDX-License-Identifier: AGPL-3.0-or-later

import { createHash } from "node:crypto";
import { mkdir, lstat, rm, writeFile } from "node:fs/promises";
import { dirname, join, posix, resolve } from "node:path";
import { promisify } from "node:util";
import { gunzip } from "node:zlib";

import { parseTarArchive } from "./archive.js";
import {
  MORPHEUS_NATIVE_ABI_VERSION,
  MORPHEUS_NATIVE_REPOSITORY,
  MORPHEUS_NATIVE_SCHEMA_VERSION,
  MORPHEUS_NATIVE_VERSION,
  selectMorpheusNativeTarget,
} from "./native-manifest.js";
import { MORPHEUS_NODE_VERSION } from "./version.js";

const gunzipAsync = promisify(gunzip);
const MAX_COMPRESSED_SIZE = 64 * 1024 * 1024;
const MAX_UNCOMPRESSED_SIZE = 256 * 1024 * 1024;
const MAX_CHECKSUM_SIZE = 4096;
const RELEASE_ASSET_HOST = "release-assets.githubusercontent.com";

function safeRelativePath(path) {
  if (path === "" || path.startsWith("/") || path.includes("\\") || path.includes("\0")) {
    throw new Error(`unsafe archive path: ${path}`);
  }
  const components = path.split("/");
  if (components.some((component) => component === "" || component === "." || component === "..")) {
    throw new Error(`unsafe archive path: ${path}`);
  }
  return components.join("/");
}

async function readLimited(response, limit, description) {
  if (!response.ok || response.body === null) {
    throw new Error(`${description} download failed: HTTP ${response.status}`);
  }
  const chunks = [];
  let size = 0;
  for await (const chunk of response.body) {
    size += chunk.length;
    if (size > limit) throw new Error(`${description} exceeds size limit`);
    chunks.push(chunk);
  }
  return Buffer.concat(chunks, size);
}

async function fetchReleaseAsset(url, limit, description, fetcher) {
  let response = await fetcher(url, { redirect: "manual" });
  if ([301, 302, 303, 307, 308].includes(response.status)) {
    const location = response.headers.get("location");
    if (location === null) throw new Error(`${description} redirect is missing`);
    const redirect = new URL(location, url);
    if (redirect.protocol !== "https:" || redirect.hostname !== RELEASE_ASSET_HOST ||
        redirect.username !== "" || redirect.password !== "") {
      throw new Error(`${description} has unsafe redirect target`);
    }
    response = await fetcher(redirect.href, { redirect: "error" });
  }
  return readLimited(response, limit, description);
}

function checksumFromSidecar(sidecar, asset) {
  const text = new TextDecoder().decode(sidecar).trim();
  const match = /^([0-9a-fA-F]{64})\s+\*?([^\s]+)$/.exec(text);
  if (match === null || match[2] !== asset) {
    throw new Error("invalid native archive checksum sidecar");
  }
  return match[1].toLowerCase();
}

async function extractNativeArchive(archive, directory, target) {
  const paths = new Set();
  const regularFiles = new Map();
  const symlinks = new Map();
  for (const entry of parseTarArchive(archive)) {
    if (!entry.path.startsWith(target.archiveRoot)) {
      throw new Error(`unexpected native archive root: ${entry.path}`);
    }
    const withoutRoot = entry.path.slice(target.archiveRoot.length);
    if (withoutRoot === "") continue;
    const relative = safeRelativePath(withoutRoot.replace(/\/$/, ""));
    if (paths.has(relative)) throw new Error(`duplicate archive path: ${relative}`);
    paths.add(relative);
    const destination = join(directory, ...relative.split("/"));
    if (entry.type === "5") {
      await mkdir(destination, { recursive: true, mode: entry.mode & 0o777 });
    } else if (entry.type === "0" || entry.type === "\0") {
      await mkdir(dirname(destination), { recursive: true });
      await writeFile(destination, entry.content, { flag: "wx", mode: entry.mode & 0o777 });
      regularFiles.set(relative, { content: entry.content, mode: entry.mode & 0o777 });
    } else if (entry.type === "2") {
      if (entry.linkPath === "" || entry.linkPath.startsWith("/") ||
          entry.linkPath.includes("\\") || entry.linkPath.includes("\0")) {
        throw new Error(`unsafe symbolic link: ${relative}`);
      }
      const normalized = posix.normalize(posix.join(posix.dirname(relative), entry.linkPath));
      if (normalized === ".." || normalized.startsWith("../")) {
        throw new Error(`symbolic link escapes output: ${relative}`);
      }
      symlinks.set(relative, normalized);
    } else {
      throw new Error(`unsupported native archive entry type: ${entry.type}`);
    }
  }
  if (!regularFiles.has(target.libraryPath) && !symlinks.has(target.libraryPath)) {
    throw new Error(`native archive is missing ${target.libraryPath}`);
  }
  function resolveRegularFile(path, visited = new Set()) {
    const regular = regularFiles.get(path);
    if (regular !== undefined) return regular;
    const linked = symlinks.get(path);
    if (linked === undefined) throw new Error(`symbolic link target is absent or not regular: ${path}`);
    if (visited.has(path)) throw new Error(`symbolic link cycle in native archive: ${path}`);
    visited.add(path);
    const resolved = resolveRegularFile(linked, visited);
    visited.delete(path);
    return resolved;
  }
  for (const [path] of symlinks) {
    const source = resolveRegularFile(path);
    const destination = join(directory, ...path.split("/"));
    await mkdir(dirname(destination), { recursive: true });
    await writeFile(destination, source.content, { flag: "wx", mode: source.mode });
  }
}

async function pathExists(path) {
  try {
    await lstat(path);
    return true;
  } catch (error) {
    if (error?.code === "ENOENT") return false;
    throw error;
  }
}

export async function acquireMorpheusNativeWithDependencies(options, dependencies) {
  const target = selectMorpheusNativeTarget(
    dependencies.platform,
    dependencies.arch,
    dependencies.glibcVersion,
  );
  const output = resolve(options.output);
  if (await pathExists(output)) throw new Error(`output path already exists: ${output}`);
  await mkdir(output);
  try {
    const sourceUrl = `${MORPHEUS_NATIVE_REPOSITORY}/releases/download/v${MORPHEUS_NATIVE_VERSION}/${target.asset}`;
    const [compressed, sidecar] = await Promise.all([
      fetchReleaseAsset(sourceUrl, MAX_COMPRESSED_SIZE, "native archive", dependencies.fetch),
      fetchReleaseAsset(`${sourceUrl}.sha256`, MAX_CHECKSUM_SIZE, "native archive checksum", dependencies.fetch),
    ]);
    const archiveSha256 = createHash("sha256").update(compressed).digest("hex");
    if (archiveSha256 !== checksumFromSidecar(sidecar, target.asset)) {
      throw new Error("native archive checksum mismatch");
    }
    const archive = await gunzipAsync(compressed, { maxOutputLength: MAX_UNCOMPRESSED_SIZE });
    await extractNativeArchive(archive, output, target);
    const receipt = {
      schema: MORPHEUS_NATIVE_SCHEMA_VERSION,
      packageVersion: MORPHEUS_NODE_VERSION,
      nativeVersion: MORPHEUS_NATIVE_VERSION,
      abiVersion: MORPHEUS_NATIVE_ABI_VERSION,
      target: target.name,
      asset: target.asset,
      archiveSha256,
      sourceUrl,
      libraryPath: target.libraryPath,
    };
    await writeFile(join(output, "MORPHEUS-NATIVE.json"), `${JSON.stringify(receipt, null, 2)}\n`, { flag: "wx" });
    return receipt;
  } catch (error) {
    try {
      await rm(output, { recursive: true, force: true });
    } catch (cleanupError) {
      throw new AggregateError([error, cleanupError], `native acquisition and cleanup both failed for ${output}`);
    }
    throw error;
  }
}

export function defaultNativeAcquisitionDependencies() {
  return {
    platform: process.platform,
    arch: process.arch,
    glibcVersion: process.report?.getReport()?.header?.glibcVersionRuntime,
    fetch,
  };
}
