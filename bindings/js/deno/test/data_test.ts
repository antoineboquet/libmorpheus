// SPDX-License-Identifier: AGPL-3.0-or-later

import { parseMorpheusDataArgs } from "../data.ts";
import { parseTarArchive, sha256 } from "../internal/data_internal.ts";
import { buildGenerIndex } from "../internal/gener_index_internal.ts";

function assert(
  condition: unknown,
  message = "assertion failed",
): asserts condition {
  if (!condition) throw new Error(message);
}

function assertThrows(callback: () => unknown, pattern: RegExp): void {
  try {
    callback();
  } catch (error) {
    assert(error instanceof Error);
    assert(pattern.test(error.message), `unexpected error: ${error.message}`);
    return;
  }
  throw new Error("expected callback to throw");
}

function putOctal(
  target: Uint8Array,
  offset: number,
  length: number,
  value: number,
): void {
  const text = value.toString(8).padStart(length - 1, "0");
  target.set(new TextEncoder().encode(`${text}\0`), offset);
}

function tarEntry(path: string, content: Uint8Array, type = "0"): Uint8Array {
  const encoder = new TextEncoder();
  const header = new Uint8Array(512);
  header.set(encoder.encode(path), 0);
  putOctal(header, 100, 8, 0o644);
  putOctal(header, 108, 8, 0);
  putOctal(header, 116, 8, 0);
  putOctal(header, 124, 12, content.length);
  putOctal(header, 136, 12, 0);
  header.fill(0x20, 148, 156);
  header[156] = type.charCodeAt(0);
  header.set(encoder.encode("ustar\0"), 257);
  header.set(encoder.encode("00"), 263);
  let checksum = 0;
  for (const byte of header) checksum += byte;
  const checksumText = checksum.toString(8).padStart(6, "0");
  header.set(encoder.encode(`${checksumText}\0 `), 148);
  const result = new Uint8Array(512 + Math.ceil(content.length / 512) * 512);
  result.set(header);
  result.set(content, 512);
  return result;
}

function tarArchive(entries: Uint8Array[]): Uint8Array {
  const size = entries.reduce((total, entry) => total + entry.length, 1024);
  const result = new Uint8Array(size);
  let offset = 0;
  for (const entry of entries) {
    result.set(entry, offset);
    offset += entry.length;
  }
  return result;
}

Deno.test("data CLI parses explicit datasets and permissions-independent options", () => {
  const options = parseMorpheusDataArgs([
    "--dataset",
    "alpheios",
    "--with-gener",
    "--output=runtime-data",
  ]);
  assert(options.dataset === "alpheios");
  assert(options.withGener === true);
  assert(options.output === "runtime-data");
  assert(options.help === false);
});

Deno.test("data CLI rejects incomplete and incompatible requests", () => {
  assertThrows(() => parseMorpheusDataArgs([]), /--dataset is required/);
  assertThrows(
    () =>
      parseMorpheusDataArgs([
        "--dataset=perseids",
        "--with-gener",
        "--output=x",
      ]),
    /only for alpheios/,
  );
  assertThrows(
    () => parseMorpheusDataArgs(["--dataset=unknown", "--output=x"]),
    /unsupported dataset/,
  );
});

Deno.test("tar reader preserves regular files and rejects corruption", () => {
  const content = new TextEncoder().encode("morpheus\n");
  const archive = tarArchive([tarEntry("root/Greek/data", content)]);
  const entries = parseTarArchive(archive);
  assert(entries.length === 1);
  assert(entries[0].path === "root/Greek/data");
  assert(new TextDecoder().decode(entries[0].content) === "morpheus\n");
  archive[0] ^= 1;
  assertThrows(() => parseTarArchive(archive), /checksum mismatch/);
});

Deno.test("SHA-256 helper uses lowercase canonical hexadecimal", async () => {
  const digest = await sha256(new TextEncoder().encode("abc"));
  assert(
    digest ===
      "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
  );
});

Deno.test("TypeScript generation index matches the native fixture", async () => {
  const source = `:le:zeta
:no:zet os_ou masc

:le:lo/-_^+gos
:no:log os_ou masc
:aj:logik os_h_on

:le:dupe
:wd:du/pe indecl

:le:dupe
:vb:du/pe w_stem pres ind act 1st sg

:le:dupe
:wd:du/pe indecl

:le:empty
:def:metadata only
`;
  const index = buildGenerIndex([source]);
  assert(index.length === 427);
  assert(
    await sha256(index) ===
      "16c6c9bdca30cb6cc0a9eb1073336ac4a828d5e3aef7b53d55ca0171550ffa9b",
  );
  assertThrows(
    () => buildGenerIndex([":le:test\n:de:unexpanded\n"]),
    /unexpanded generation record/,
  );
});
