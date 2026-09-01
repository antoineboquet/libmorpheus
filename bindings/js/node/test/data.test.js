// SPDX-License-Identifier: AGPL-3.0-or-later

import assert from "node:assert/strict";
import { mkdir, mkdtemp, readFile, rm } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { gzipSync } from "node:zlib";

import { acquireMorpheusDataWithDependencies, sha256 } from "../internal/data-internal.js";
import { parseMorpheusDataArgs } from "../data.js";

function writeString(buffer, offset, length, value) {
  buffer.write(value, offset, Math.min(length, Buffer.byteLength(value)), "ascii");
}

function tarEntry(path, content = Buffer.alloc(0), type = "0") {
  const header = Buffer.alloc(512);
  writeString(header, 0, 100, path);
  writeString(header, 100, 8, "0000644\0");
  writeString(header, 108, 8, "0000000\0");
  writeString(header, 116, 8, "0000000\0");
  writeString(header, 124, 12, `${content.length.toString(8).padStart(11, "0")}\0`);
  writeString(header, 136, 12, "00000000000\0");
  header.fill(0x20, 148, 156);
  writeString(header, 156, 1, type);
  writeString(header, 257, 6, "ustar\0");
  writeString(header, 263, 2, "00");
  const checksum = header.reduce((sum, byte) => sum + byte, 0);
  writeString(header, 148, 8, `${checksum.toString(8).padStart(6, "0")}\0 `);
  return Buffer.concat([header, content, Buffer.alloc((512 - content.length % 512) % 512)]);
}

function fixture() {
  const file = Buffer.from("fixture stem\n");
  const license = Buffer.from("fixture license\n");
  const archive = Buffer.concat([
    tarEntry("fixture/LICENSE", license),
    tarEntry("fixture/stemlib/Greek/steminds/nomind", file),
    Buffer.alloc(1024),
  ]);
  const line = `${sha256(file)}  Greek/steminds/nomind\n`;
  const definition = {
    name: "alpheios",
    repository: "https://example.invalid/source",
    revision: "fixture-revision",
    archiveUrl: "https://example.invalid/archive.tar.gz",
    archiveRoot: "fixture/",
    dataPrefix: "stemlib/",
    fileCount: 1,
    treeSha256: sha256(new TextEncoder().encode(line)),
    licensePath: "LICENSE",
    licenseSha256: sha256(license),
    languages: ["grc"],
    generation: true,
  };
  return { compressed: gzipSync(archive), definition };
}

test("data acquisition verifies the selected tree and preserves its license", async () => {
  await mkdir(tmpdir(), { recursive: true });
  const parent = await mkdtemp(join(tmpdir(), "libmorpheus-node-data-"));
  const output = join(parent, "data");
  try {
    const { compressed, definition } = fixture();
    const receipt = await acquireMorpheusDataWithDependencies(
      { dataset: "alpheios", output },
      {
        datasets: { alpheios: definition },
        fetch: async () => new Response(compressed),
      },
    );
    assert.equal(receipt.files.count, 1);
    assert.deepEqual(receipt.languages, ["grc"]);
    assert.equal(await readFile(join(output, "Greek/steminds/nomind"), "utf8"), "fixture stem\n");
    assert.equal(await readFile(join(output, "UPSTREAM-LICENSE"), "utf8"), "fixture license\n");
  } finally {
    await rm(parent, { recursive: true, force: true });
  }
});

test("data CLI selects only pinned supported datasets", () => {
  assert.deepEqual(
    parseMorpheusDataArgs(["--dataset=perseids", "--output", "data"]),
    { dataset: "perseids", output: "data", withGener: false, help: false },
  );
  assert.throws(() => parseMorpheusDataArgs(["--dataset", "latest", "--output", "data"]), /unsupported dataset/);
  assert.throws(() => parseMorpheusDataArgs(["--dataset", "perseids", "--output", "data", "--with-gener"]), /only for alpheios/);
  assert.equal(
    parseMorpheusDataArgs(["--dataset", "alpheios", "--output", "data", "--with-gener"]).withGener,
    true,
  );
});
