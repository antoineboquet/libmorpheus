// SPDX-License-Identifier: AGPL-3.0-or-later

import { init } from "./init.ts";

function assert(condition: unknown, message: string): asserts condition {
  if (!condition) throw new Error(message);
}

function assertThrows(callback: () => unknown, pattern: RegExp): void {
  try {
    callback();
  } catch (error) {
    assert(error instanceof Error, "non-Error exception");
    assert(pattern.test(error.message), `unexpected error: ${error.message}`);
    return;
  }
  throw new Error("expected callback to throw");
}

Deno.test("init describes the default Alpheios generation installation", () => {
  const messages: string[] = [];
  const plan = init({ write: (message) => messages.push(message) });
  assert(plan.dataset === "alpheios", "unexpected default dataset");
  assert(plan.withGener, "default plan omits the generation index");
  assert(plan.nativeCommand.includes("/native"), "native command is missing");
  assert(plan.dataCommand.includes("--with-gener"), "gener flag is missing");
  assert(messages.length === 1, "guide was not written exactly once");
  assert(
    messages[0].includes("--allow-ffi"),
    "FFI permission is not explained",
  );
});

Deno.test("init describes Perseids Greek and Latin analysis", () => {
  const plan = init({
    dataset: "perseids",
    nativeOutput: "./native files",
    dataOutput: "./Greek and Latin",
    write: () => undefined,
  });
  assert(!plan.withGener, "Perseids unexpectedly enables generation");
  assert(plan.dataCommand.includes("--dataset perseids"), "dataset is missing");
  assert(!plan.dataCommand.includes("--with-gener"), "gener flag leaked");
  assert(plan.nativeCommand.includes("'./native files'"), "path is unquoted");
  assert(plan.dataCommand.includes("'./Greek and Latin'"), "path is unquoted");
});

Deno.test("init rejects unsupported and empty installation choices", () => {
  assertThrows(
    () =>
      init({
        dataset: "unknown" as "alpheios",
        write: () => undefined,
      }),
    /unsupported dataset/,
  );
  assertThrows(
    () =>
      init({ dataset: "perseids", withGener: true, write: () => undefined }),
    /only with the Alpheios dataset/,
  );
  assertThrows(
    () => init({ dataOutput: " ", write: () => undefined }),
    /dataOutput must not be empty/,
  );
});
