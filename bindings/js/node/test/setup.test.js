// SPDX-License-Identifier: AGPL-3.0-or-later

import assert from "node:assert/strict";
import { resolve } from "node:path";
import test from "node:test";

import { setupMorpheusWithDependencies } from "../internal/setup-internal.js";
import { parseMorpheusSetupArgs } from "../setup.js";

function nativeReceipt() {
  return { libraryPath: "lib/libmorpheus.so", target: "fixture" };
}

test("combined setup returns resolved runtime paths", async () => {
  const calls = [];
  const receipt = await setupMorpheusWithDependencies(
    {
      dataset: "alpheios",
      withGener: true,
      nativeOutput: "fixture-native",
      dataOutput: "fixture-data",
    },
    {
      pathExists: async () => false,
      remove: async () => assert.fail("unexpected rollback"),
      acquireNative: async (options) => {
        calls.push(["native", options]);
        return nativeReceipt();
      },
      acquireData: async (options) => {
        calls.push(["data", options]);
        return { dataset: "alpheios" };
      },
    },
  );
  assert.equal(receipt.nativeLibraryPath, resolve("fixture-native/lib/libmorpheus.so"));
  assert.deepEqual(calls[1][1], {
    dataset: "alpheios",
    output: resolve("fixture-data"),
    withGener: true,
  });
});

test("combined setup rolls back native acquisition when data fails", async () => {
  const removed = [];
  await assert.rejects(
    setupMorpheusWithDependencies(
      { dataset: "perseids", nativeOutput: "fixture-native", dataOutput: "fixture-data" },
      {
        pathExists: async () => false,
        remove: async (path) => removed.push(path),
        acquireNative: async () => nativeReceipt(),
        acquireData: async () => { throw new Error("data failed"); },
      },
    ),
    /data failed/,
  );
  assert.deepEqual(removed, [resolve("fixture-native")]);
});

test("combined setup rejects overlapping outputs", async () => {
  await assert.rejects(
    setupMorpheusWithDependencies(
      { dataset: "alpheios", nativeOutput: "fixture", dataOutput: "fixture/data" },
      {},
    ),
    /non-overlapping/,
  );
});

test("setup CLI keeps explicit acquisition choices", () => {
  assert.deepEqual(
    parseMorpheusSetupArgs([
      "--dataset=alpheios",
      "--with-gener",
      "--native-output", "native",
      "--data-output=data",
    ]),
    {
      dataset: "alpheios",
      nativeOutput: "native",
      dataOutput: "data",
      withGener: true,
      help: false,
    },
  );
});
