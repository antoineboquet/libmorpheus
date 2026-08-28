// SPDX-License-Identifier: AGPL-3.0-or-later

import createGenerPreparer from "./gener_preparer.mjs";
import { buildGenerIndex } from "./gener_index_internal.ts";
import {
  GENER_CORPUS_EXCEPTIONS,
  GENER_CORPUS_FILES,
  GENER_INDEX_SHA256,
  GENER_PREPARED_SHA256,
} from "./gener_manifest.ts";

interface EmscriptenFileSystem {
  mkdirTree(path: string): void;
  writeFile(path: string, content: Uint8Array | string): void;
  readFile(path: string, options?: { encoding?: "utf8" }): Uint8Array | string;
}
interface GenerPreparerModule {
  readonly FS: EmscriptenFileSystem;
  callMain(arguments_: string[]): number;
}
function parentPath(path: string): string {
  const separator = path.lastIndexOf("/");
  return separator <= 0 ? "/" : path.slice(0, separator);
}
function populateSupportStemlib(
  fileSystem: EmscriptenFileSystem,
  files: ReadonlyMap<string, Uint8Array>,
): void {
  fileSystem.mkdirTree("/morphlib/Greek");
  for (const [path, content] of files) {
    if (!path.startsWith("Greek/")) continue;
    const destination = `/morphlib/${path}`;
    fileSystem.mkdirTree(parentPath(destination));
    fileSystem.writeFile(destination, content);
  }
}

export async function prepareGenerIndex(
  alpheiosFiles: ReadonlyMap<string, Uint8Array>,
  perseidsFiles: ReadonlyMap<string, Uint8Array>,
  digest: (content: Uint8Array) => Promise<string>,
): Promise<Uint8Array> {
  const errors: string[] = [];
  const module = await createGenerPreparer({
    noInitialRun: true,
    print: () => undefined,
    printErr: (message: unknown) => errors.push(String(message)),
  }) as GenerPreparerModule;
  const fileSystem = module.FS;
  populateSupportStemlib(fileSystem, perseidsFiles);
  fileSystem.mkdirTree("/input");
  const inputs: string[] = [];
  for (const relative of GENER_CORPUS_FILES) {
    const content = alpheiosFiles.get(`Greek/${relative}`);
    if (content === undefined) {
      throw new Error(`Alpheios generation source is missing: ${relative}`);
    }
    const input = `/input/${relative}`;
    fileSystem.mkdirTree(parentPath(input));
    fileSystem.writeFile(input, content);
    inputs.push(input);
  }
  fileSystem.writeFile("/exceptions.tsv", GENER_CORPUS_EXCEPTIONS);
  const status = module.callMain([
    "--exceptions",
    "/exceptions.tsv",
    "/prepared.txt",
    ...inputs,
  ]);
  if (status !== 0) {
    const detail = errors.slice(-8).join("\n");
    throw new Error(
      `experimental generation source preparation failed (${status})` +
        (detail === "" ? "" : `:\n${detail}`),
    );
  }
  const prepared = fileSystem.readFile("/prepared.txt") as Uint8Array;
  if (await digest(prepared) !== GENER_PREPARED_SHA256) {
    throw new Error("experimental generation source digest mismatch");
  }
  const source = new TextDecoder("utf-8", { fatal: true }).decode(prepared);
  const index = buildGenerIndex([source]);
  if (await digest(index) !== GENER_INDEX_SHA256) {
    throw new Error("experimental generation index digest mismatch");
  }
  return index;
}
