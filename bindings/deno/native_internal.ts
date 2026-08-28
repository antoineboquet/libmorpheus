// SPDX-License-Identifier: AGPL-3.0-or-later

import { dirname, join, posix, resolve } from "node:path";
import { parseTarArchive, sha256 } from "./data_internal.ts";
import { MORPHEUS_PACKAGE_VERSION } from "./data_manifest.ts";
import {
  MORPHEUS_NATIVE_ABI_VERSION,
  MORPHEUS_NATIVE_REPOSITORY,
  MORPHEUS_NATIVE_SCHEMA_VERSION,
  type MorpheusNativeTarget,
  selectMorpheusNativeTarget,
} from "./native_manifest.ts";

const MAX_COMPRESSED_SIZE = 64 * 1024 * 1024;
const MAX_UNCOMPRESSED_SIZE = 256 * 1024 * 1024;
const MAX_CHECKSUM_SIZE = 4096;
const RELEASE_ASSET_HOST = "release-assets.githubusercontent.com";

export interface MorpheusNativeReceipt {
  readonly schema: number;
  readonly packageVersion: string;
  readonly abiVersion: number;
  readonly target: string;
  readonly asset: string;
  readonly archiveSha256: string;
  readonly sourceUrl: string;
  readonly libraryPath: string;
}

export interface AcquireMorpheusNativeOptions {
  readonly output: string;
}

export interface NativeAcquisitionDependencies {
  readonly os: string;
  readonly arch: string;
  readonly fetch: typeof fetch;
}

function safeRelativePath(path: string): string {
  if (
    path === "" || path.startsWith("/") || path.includes("\\") ||
    path.includes("\0")
  ) {
    throw new Error(`unsafe archive path: ${path}`);
  }
  const components = path.split("/");
  if (
    components.some((component) =>
      component === "" || component === "." || component === ".."
    )
  ) throw new Error(`unsafe archive path: ${path}`);
  return components.join("/");
}

async function readLimited(
  response: Response,
  limit: number,
  description: string,
): Promise<Uint8Array> {
  if (!response.ok || response.body === null) {
    throw new Error(`${description} download failed: HTTP ${response.status}`);
  }
  const chunks: Uint8Array[] = [];
  const reader = response.body.getReader();
  let size = 0;
  try {
    while (true) {
      const { done, value } = await reader.read();
      if (done) break;
      size += value.length;
      if (size > limit) throw new Error(`${description} exceeds size limit`);
      chunks.push(value);
    }
  } finally {
    reader.releaseLock();
  }
  const result = new Uint8Array(size);
  let offset = 0;
  for (const chunk of chunks) {
    result.set(chunk, offset);
    offset += chunk.length;
  }
  return result;
}

async function fetchReleaseAsset(
  url: string,
  limit: number,
  description: string,
  fetcher: typeof fetch,
): Promise<Uint8Array> {
  let response = await fetcher(url, { redirect: "manual" });
  if ([301, 302, 303, 307, 308].includes(response.status)) {
    const location = response.headers.get("location");
    if (location === null) {
      throw new Error(`${description} redirect is missing`);
    }
    const redirect = new URL(location, url);
    if (
      redirect.protocol !== "https:" ||
      redirect.hostname !== RELEASE_ASSET_HOST ||
      redirect.username !== "" || redirect.password !== ""
    ) throw new Error(`${description} has unsafe redirect target`);
    response = await fetcher(redirect.href, { redirect: "error" });
  }
  return await readLimited(response, limit, description);
}

function checksumFromSidecar(sidecar: Uint8Array, asset: string): string {
  const text = new TextDecoder().decode(sidecar).trim();
  const match = /^([0-9a-fA-F]{64})\s+\*?([^\s]+)$/.exec(text);
  if (match === null || match[2] !== asset) {
    throw new Error("invalid native archive checksum sidecar");
  }
  return match[1].toLowerCase();
}

async function decompressArchive(compressed: Uint8Array): Promise<Uint8Array> {
  const response = new Response(
    new Blob([Uint8Array.from(compressed).buffer]).stream().pipeThrough(
      new DecompressionStream("gzip"),
    ),
  );
  return await readLimited(
    response,
    MAX_UNCOMPRESSED_SIZE,
    "decompressed native archive",
  );
}

async function extractNativeArchive(
  archive: Uint8Array,
  directory: string,
  target: MorpheusNativeTarget,
): Promise<void> {
  const entries = parseTarArchive(archive);
  const paths = new Set<string>();
  const extracted = new Set<string>();
  const symlinks: { path: string; target: string }[] = [];

  for (const entry of entries) {
    if (!entry.path.startsWith(target.archiveRoot)) {
      throw new Error(`unexpected native archive root: ${entry.path}`);
    }
    const withoutRoot = entry.path.slice(target.archiveRoot.length);
    if (withoutRoot === "") continue;
    const relative = safeRelativePath(withoutRoot.replace(/\/$/, ""));
    if (paths.has(relative)) {
      throw new Error(`duplicate archive path: ${relative}`);
    }
    paths.add(relative);
    const destination = join(directory, ...relative.split("/"));
    if (entry.type === "5") {
      await Deno.mkdir(destination, {
        recursive: true,
        mode: entry.mode & 0o777,
      });
      extracted.add(relative);
    } else if (entry.type === "0" || entry.type === "\0") {
      await Deno.mkdir(dirname(destination), { recursive: true });
      await Deno.writeFile(destination, entry.content, {
        createNew: true,
        mode: entry.mode & 0o777,
      });
      extracted.add(relative);
    } else if (entry.type === "2") {
      if (
        entry.linkPath === "" || entry.linkPath.startsWith("/") ||
        entry.linkPath.includes("\\") || entry.linkPath.includes("\0")
      ) throw new Error(`unsafe symbolic link: ${relative}`);
      const normalized = posix.normalize(
        posix.join(posix.dirname(relative), entry.linkPath),
      );
      if (normalized === ".." || normalized.startsWith("../")) {
        throw new Error(`symbolic link escapes output: ${relative}`);
      }
      symlinks.push({ path: relative, target: entry.linkPath });
      extracted.add(relative);
    } else {
      throw new Error(`unsupported native archive entry type: ${entry.type}`);
    }
  }

  if (!extracted.has(target.libraryPath)) {
    throw new Error(`native archive is missing ${target.libraryPath}`);
  }
  for (const link of symlinks) {
    const normalized = posix.normalize(
      posix.join(posix.dirname(link.path), link.target),
    );
    if (!extracted.has(normalized)) {
      throw new Error(`symbolic link target is absent: ${link.path}`);
    }
    const destination = join(directory, ...link.path.split("/"));
    await Deno.mkdir(dirname(destination), { recursive: true });
    await Deno.symlink(link.target, destination);
  }
}

function releaseUrl(asset: string): string {
  return `${MORPHEUS_NATIVE_REPOSITORY}/releases/download/v${MORPHEUS_PACKAGE_VERSION}/${asset}`;
}

async function pathExists(path: string): Promise<boolean> {
  try {
    await Deno.lstat(path);
    return true;
  } catch (error) {
    if (error instanceof Deno.errors.NotFound) return false;
    throw error;
  }
}

export async function acquireMorpheusNativeWithDependencies(
  options: AcquireMorpheusNativeOptions,
  dependencies: NativeAcquisitionDependencies,
): Promise<MorpheusNativeReceipt> {
  const target = selectMorpheusNativeTarget(dependencies.os, dependencies.arch);
  const output = resolve(options.output);
  if (await pathExists(output)) {
    throw new Error(`output path already exists: ${output}`);
  }
  await Deno.mkdir(output);
  try {
    const sourceUrl = releaseUrl(target.asset);
    const [compressed, sidecar] = await Promise.all([
      fetchReleaseAsset(
        sourceUrl,
        MAX_COMPRESSED_SIZE,
        "native archive",
        dependencies.fetch,
      ),
      fetchReleaseAsset(
        `${sourceUrl}.sha256`,
        MAX_CHECKSUM_SIZE,
        "native archive checksum",
        dependencies.fetch,
      ),
    ]);
    const archiveSha256 = await sha256(compressed);
    if (archiveSha256 !== checksumFromSidecar(sidecar, target.asset)) {
      throw new Error("native archive checksum mismatch");
    }
    await extractNativeArchive(
      await decompressArchive(compressed),
      output,
      target,
    );
    const receipt: MorpheusNativeReceipt = {
      schema: MORPHEUS_NATIVE_SCHEMA_VERSION,
      packageVersion: MORPHEUS_PACKAGE_VERSION,
      abiVersion: MORPHEUS_NATIVE_ABI_VERSION,
      target: target.name,
      asset: target.asset,
      archiveSha256,
      sourceUrl,
      libraryPath: target.libraryPath,
    };
    await Deno.writeTextFile(
      join(output, "MORPHEUS-NATIVE.json"),
      `${JSON.stringify(receipt, null, 2)}\n`,
      { createNew: true },
    );
    return receipt;
  } catch (error) {
    try {
      await Deno.remove(output, { recursive: true });
    } catch (cleanupError) {
      if (!(cleanupError instanceof Deno.errors.NotFound)) {
        throw new AggregateError(
          [error, cleanupError],
          `native acquisition and cleanup both failed for ${output}`,
        );
      }
    }
    throw error;
  }
}

export function defaultNativeAcquisitionDependencies(): NativeAcquisitionDependencies {
  return { os: Deno.build.os, arch: Deno.build.arch, fetch };
}
