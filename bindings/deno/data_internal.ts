// SPDX-License-Identifier: AGPL-3.0-or-later

import { dirname, join, resolve } from "node:path";
import {
  MORPHEUS_DATA_SCHEMA_VERSION,
  MORPHEUS_DATASETS,
  type MorpheusDatasetDefinition,
  type MorpheusDatasetName,
} from "./data_manifest.ts";
import { prepareGenerIndex } from "./gener_runtime_internal.ts";
import { MORPHEUS_DENO_VERSION } from "./version.ts";

const TAR_BLOCK_SIZE = 512;
const MAX_COMPRESSED_SIZE = 64 * 1024 * 1024;
const MAX_UNCOMPRESSED_SIZE = 256 * 1024 * 1024;
const textDecoder = new TextDecoder();
const textEncoder = new TextEncoder();

/** Verified provenance written to `MORPHEUS-DATA.json`. */
export interface MorpheusDataReceipt {
  /** Receipt schema version. */
  readonly schema: number;
  /** JSR package version that performed the acquisition. */
  readonly packageVersion: string;
  /** Installed dataset name. */
  readonly dataset: MorpheusDatasetName;
  /** ISO language codes covered by the dataset. */
  readonly languages: readonly string[];
  /** Pinned upstream archive provenance. */
  readonly source: {
    /** Original upstream repository. */
    readonly repository: string;
    /** Immutable upstream commit. */
    readonly revision: string;
    /** Downloaded archive URL. */
    readonly archiveUrl: string;
  };
  /** Verified stem-data tree metadata. */
  readonly files: {
    /** Number of installed stem files. */
    readonly count: number;
    /** Deterministic digest of the installed tree. */
    readonly treeSha256: string;
  };
  /** Experimental generation-index provenance and availability. */
  readonly generation: {
    /** Constant marker that generation is experimental. */
    readonly experimental: true;
    /** Whether `gener.index` was built and installed. */
    readonly available: boolean;
    /** Digest of `gener.index`, or `null` when it was not requested. */
    readonly indexSha256: string | null;
    /** Supplemental source provenance, or `null` when unused. */
    readonly supportSource: {
      /** Original supplemental repository. */
      readonly repository: string;
      /** Immutable supplemental commit. */
      readonly revision: string;
      /** Downloaded supplemental archive URL. */
      readonly archiveUrl: string;
    } | null;
  };
}

/** Options for acquiring one pinned stem dataset. */
export interface AcquireMorpheusDataOptions {
  /** Upstream dataset to verify and install. */
  readonly dataset: MorpheusDatasetName;
  /** New destination directory; existing paths are refused. */
  readonly output: string;
  /** Build the experimental Greek index; supported only by Alpheios. */
  readonly withGener?: boolean;
}

interface ArchiveDataset {
  readonly files: ReadonlyMap<string, Uint8Array>;
  readonly license: Uint8Array;
}

export interface ArchiveEntry {
  readonly path: string;
  readonly type: string;
  readonly content: Uint8Array;
  readonly mode: number;
  readonly linkPath: string;
}

function decodeTarString(bytes: Uint8Array): string {
  const zero = bytes.indexOf(0);
  return textDecoder.decode(zero === -1 ? bytes : bytes.subarray(0, zero));
}

function parseTarNumber(bytes: Uint8Array, field: string): number {
  if ((bytes[0] & 0x80) !== 0) {
    let value = BigInt(bytes[0] & 0x7f);
    for (const byte of bytes.subarray(1)) value = (value << 8n) | BigInt(byte);
    if (value > BigInt(Number.MAX_SAFE_INTEGER)) {
      throw new Error(`tar ${field} exceeds JavaScript limits`);
    }
    return Number(value);
  }
  const source = decodeTarString(bytes).trim();
  if (source === "") return 0;
  if (!/^[0-7]+$/.test(source)) throw new Error(`invalid tar ${field}`);
  const value = Number.parseInt(source, 8);
  if (!Number.isSafeInteger(value)) throw new Error(`invalid tar ${field}`);
  return value;
}

function verifyTarChecksum(header: Uint8Array): void {
  const expected = parseTarNumber(header.subarray(148, 156), "checksum");
  let actual = 0;
  for (let index = 0; index < header.length; index++) {
    actual += index >= 148 && index < 156 ? 0x20 : header[index];
  }
  if (actual !== expected) throw new Error("tar header checksum mismatch");
}

function isZeroBlock(block: Uint8Array): boolean {
  return block.every((byte) => byte === 0);
}

function parsePax(content: Uint8Array): Map<string, string> {
  const fields = new Map<string, string>();
  let offset = 0;
  while (offset < content.length) {
    const space = content.indexOf(0x20, offset);
    if (space === -1) throw new Error("invalid PAX record length");
    const lengthText = textDecoder.decode(content.subarray(offset, space));
    if (!/^[1-9][0-9]*$/.test(lengthText)) {
      throw new Error("invalid PAX record length");
    }
    const length = Number.parseInt(lengthText, 10);
    const end = offset + length;
    if (
      !Number.isSafeInteger(length) || end > content.length ||
      content[end - 1] !== 0x0a
    ) {
      throw new Error("truncated PAX record");
    }
    const record = textDecoder.decode(content.subarray(space + 1, end - 1));
    const equals = record.indexOf("=");
    if (equals <= 0) throw new Error("invalid PAX record");
    fields.set(record.slice(0, equals), record.slice(equals + 1));
    offset = end;
  }
  return fields;
}

export function parseTarArchive(archive: Uint8Array): ArchiveEntry[] {
  const entries: ArchiveEntry[] = [];
  let offset = 0;
  let nextPax = new Map<string, string>();
  let globalPax = new Map<string, string>();
  let longPath: string | undefined;
  let longLinkPath: string | undefined;
  let zeroBlocks = 0;

  while (offset + TAR_BLOCK_SIZE <= archive.length) {
    const header = archive.subarray(offset, offset + TAR_BLOCK_SIZE);
    offset += TAR_BLOCK_SIZE;
    if (isZeroBlock(header)) {
      zeroBlocks++;
      if (zeroBlocks === 2) return entries;
      continue;
    }
    zeroBlocks = 0;
    verifyTarChecksum(header);
    const size = parseTarNumber(header.subarray(124, 136), "size");
    const paddedSize = Math.ceil(size / TAR_BLOCK_SIZE) * TAR_BLOCK_SIZE;
    if (offset + paddedSize > archive.length) {
      throw new Error("truncated tar entry");
    }
    const content = archive.subarray(offset, offset + size);
    offset += paddedSize;
    const type = String.fromCharCode(header[156] || 0x30);
    const mode = parseTarNumber(header.subarray(100, 108), "mode");
    const name = decodeTarString(header.subarray(0, 100));
    const prefix = decodeTarString(header.subarray(345, 500));
    const headerPath = prefix === "" ? name : `${prefix}/${name}`;

    if (type === "x") {
      nextPax = parsePax(content);
      continue;
    }
    if (type === "g") {
      globalPax = new Map([...globalPax, ...parsePax(content)]);
      continue;
    }
    if (type === "L") {
      longPath = decodeTarString(content);
      continue;
    }
    if (type === "K") {
      longLinkPath = decodeTarString(content);
      continue;
    }
    const path = nextPax.get("path") ?? globalPax.get("path") ?? longPath ??
      headerPath;
    const linkPath = nextPax.get("linkpath") ?? globalPax.get("linkpath") ??
      longLinkPath ?? decodeTarString(header.subarray(157, 257));
    nextPax = new Map();
    longPath = undefined;
    longLinkPath = undefined;
    entries.push({ path, type, content, mode, linkPath });
  }
  throw new Error("tar archive has no complete end marker");
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
      component === "" || component === "." ||
      component === ".."
    )
  ) {
    throw new Error(`unsafe archive path: ${path}`);
  }
  return components.join("/");
}

export async function sha256(bytes: Uint8Array): Promise<string> {
  const owned = Uint8Array.from(bytes);
  const digest = await crypto.subtle.digest("SHA-256", owned.buffer);
  return Array.from(
    new Uint8Array(digest),
    (byte) => byte.toString(16).padStart(2, "0"),
  ).join("");
}

async function treeDigest(lines: string[]): Promise<string> {
  lines.sort((left, right) => {
    const leftPath = left.slice(66);
    const rightPath = right.slice(66);
    return leftPath < rightPath ? -1 : leftPath === rightPath ? 0 : 1;
  });
  return await sha256(textEncoder.encode(`${lines.join("\n")}\n`));
}

async function extractArchiveDataset(
  archive: Uint8Array,
  definition: MorpheusDatasetDefinition,
): Promise<ArchiveDataset> {
  const dataRoot = `${definition.archiveRoot}${definition.dataPrefix}`;
  const licensePath = `${definition.archiveRoot}${definition.licensePath}`;
  const paths = new Set<string>();
  const files = new Map<string, Uint8Array>();
  const digestLines: string[] = [];
  let license: Uint8Array | undefined;

  for (const entry of parseTarArchive(archive)) {
    if (entry.path === licensePath) {
      if (entry.type !== "0") throw new Error("upstream license is not a file");
      license = entry.content;
      continue;
    }
    if (!entry.path.startsWith(dataRoot)) continue;
    const relative = entry.path.slice(dataRoot.length).replace(/\/$/, "");
    if (relative === "") continue;
    const path = safeRelativePath(relative);
    if (entry.type === "5") continue;
    if (entry.type !== "0") {
      throw new Error(`unsupported dataset archive entry: ${entry.path}`);
    }
    if (paths.has(path)) throw new Error(`duplicate dataset path: ${path}`);
    paths.add(path);
    files.set(path, entry.content);
    const digest = await sha256(entry.content);
    digestLines.push(`${digest}  ${path}`);
  }

  if (paths.size !== definition.fileCount) {
    throw new Error(
      `${definition.name} archive contains ${paths.size} dataset files; expected ${definition.fileCount}`,
    );
  }
  const actualTree = await treeDigest(digestLines);
  if (actualTree !== definition.treeSha256) {
    throw new Error(
      `${definition.name} dataset digest mismatch: ${actualTree}`,
    );
  }
  if (
    license === undefined ||
    await sha256(license) !== definition.licenseSha256
  ) {
    throw new Error(`${definition.name} upstream license digest mismatch`);
  }
  return { files, license };
}

async function writeArchiveDataset(
  dataset: ArchiveDataset,
  directory: string,
): Promise<void> {
  for (const [path, content] of dataset.files) {
    const destination = join(directory, ...path.split("/"));
    await Deno.mkdir(dirname(destination), { recursive: true });
    await Deno.writeFile(destination, content, { createNew: true });
  }
  await Deno.writeFile(join(directory, "UPSTREAM-LICENSE"), dataset.license, {
    createNew: true,
  });
}

async function readLimited(
  stream: ReadableStream<Uint8Array>,
  limit: number,
  description: string,
): Promise<Uint8Array> {
  const chunks: Uint8Array[] = [];
  const reader = stream.getReader();
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

async function downloadArchive(
  definition: MorpheusDatasetDefinition,
): Promise<Uint8Array> {
  const response = await fetch(definition.archiveUrl, { redirect: "error" });
  if (!response.ok || response.body === null) {
    throw new Error(
      `cannot download ${definition.name} dataset: HTTP ${response.status}`,
    );
  }
  const compressed = await readLimited(
    response.body,
    MAX_COMPRESSED_SIZE,
    "compressed dataset archive",
  );
  const decompressed = new Blob([Uint8Array.from(compressed).buffer]).stream()
    .pipeThrough(
      new DecompressionStream("gzip"),
    );
  return await readLimited(
    decompressed,
    MAX_UNCOMPRESSED_SIZE,
    "decompressed dataset archive",
  );
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

/** Downloads, verifies, and installs stem data without a native toolchain. */
export async function acquireMorpheusData(
  options: AcquireMorpheusDataOptions,
): Promise<MorpheusDataReceipt> {
  const definition = MORPHEUS_DATASETS[options.dataset];
  if (definition === undefined) {
    throw new TypeError(`unsupported Morpheus dataset: ${options.dataset}`);
  }
  if (options.withGener && !definition.generation) {
    throw new TypeError(
      `dataset ${definition.name} does not support experimental generation`,
    );
  }
  const output = resolve(options.output);
  if (await pathExists(output)) {
    throw new Error(`output path already exists: ${output}`);
  }
  await Deno.mkdir(output);
  try {
    const archive = await downloadArchive(definition);
    const dataset = await extractArchiveDataset(archive, definition);
    await writeArchiveDataset(dataset, output);
    let indexSha256: string | null = null;
    let supportSource: MorpheusDataReceipt["generation"]["supportSource"] =
      null;
    if (options.withGener) {
      const supportDefinition = MORPHEUS_DATASETS.perseids;
      const supportArchive = await downloadArchive(supportDefinition);
      const supportDataset = await extractArchiveDataset(
        supportArchive,
        supportDefinition,
      );
      const index = await prepareGenerIndex(
        dataset.files,
        supportDataset.files,
        sha256,
      );
      indexSha256 = await sha256(index);
      await Deno.writeFile(join(output, "gener.index"), index, {
        createNew: true,
      });
      supportSource = {
        repository: supportDefinition.repository,
        revision: supportDefinition.revision,
        archiveUrl: supportDefinition.archiveUrl,
      };
    }
    const receipt: MorpheusDataReceipt = {
      schema: MORPHEUS_DATA_SCHEMA_VERSION,
      packageVersion: MORPHEUS_DENO_VERSION,
      dataset: definition.name,
      languages: definition.languages,
      source: {
        repository: definition.repository,
        revision: definition.revision,
        archiveUrl: definition.archiveUrl,
      },
      files: {
        count: definition.fileCount,
        treeSha256: definition.treeSha256,
      },
      generation: {
        experimental: true,
        available: options.withGener === true,
        indexSha256,
        supportSource,
      },
    };
    await Deno.writeTextFile(
      join(output, "MORPHEUS-DATA.json"),
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
          `data acquisition and cleanup both failed for ${output}`,
        );
      }
    }
    throw error;
  }
}
