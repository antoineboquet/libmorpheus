// SPDX-License-Identifier: AGPL-3.0-or-later

import createGenerPreparer from "./gener_preparer.mjs";
import process from "node:process";

function assert(condition: unknown, message: string): asserts condition {
  if (!condition) throw new Error(message);
}
interface TestModule {
  readonly FS: {
    writeFile(path: string, content: string): void;
    readFile(path: string, options: { encoding: "utf8" }): string;
  };
  callMain(arguments_: string[]): number;
}
async function prepare(
  source: string,
): Promise<{ readonly status: number; readonly output: string | null }> {
  const module = await createGenerPreparer({
    noInitialRun: true,
    print: () => undefined,
    printErr: () => undefined,
  }) as TestModule;
  module.FS.writeFile("/input.txt", source);
  const previousExitCode = process.exitCode;
  let status: number;
  try {
    status = module.callMain(["/output.txt", "/input.txt"]);
  } finally {
    process.exitCode = previousExitCode;
  }
  let output: string | null = null;
  try {
    output = module.FS.readFile("/output.txt", { encoding: "utf8" });
  } catch {
    // A failed preparer must not leave a partial output.
  }
  return { status, output };
}

Deno.test("WebAssembly generation preparer preserves continuations", async () => {
  const input = `:le:a)gakleh/s
:aj:a)ga klehs_kleous,suff_acc late
@ end:kle/a acc sg masc fem epic
@ ionic
:def:ignored metadata
@ end:kle/as acc pl masc fem epic

:le:dupe
:wd:du/pe indecl
@

:le:verb
:vs:lu_ s_aor act
@ mid epic
`;
  const expected = `:le:a)gakleh/s
:aj:a)ga klehs_kleous,suff_acc late
:aj:a)ga klehs_kleous,suff_acc end:kle/a acc sg masc fem epic
:aj:a)ga klehs_kleous,suff_acc ionic
:aj:a)ga klehs_kleous,suff_acc end:kle/as acc pl masc fem epic
:le:dupe
:wd:du/pe indecl
:wd:du/pe indecl
:le:verb
:vs:lu_ s_aor act
:vs:lu_ s_aor mid epic
`;
  const first = await prepare(input);
  const second = await prepare(input);
  assert(first.status === 0, `first preparer status: ${first.status}`);
  assert(second.status === 0, `second preparer status: ${second.status}`);
  assert(first.output === expected, "first prepared source differs");
  assert(second.output === expected, "isolated prepared source differs");
});

Deno.test("WebAssembly generation preparer removes failed output", async () => {
  const result = await prepare(":le:orphan\n-:vs:disabled w_stem\n@ inf act\n");
  assert(result.status !== 0, "invalid source unexpectedly succeeded");
  assert(result.output === null, "failed preparer left an output");
});
