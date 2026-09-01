// SPDX-License-Identifier: AGPL-3.0-or-later

import assert from "node:assert/strict";
import { mkdir, mkdtemp, readFile, readdir, rm } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";

import { stageNpmBootstrapPackages } from "../bootstrap-npm.mjs";

test("stages inert bootstrap versions for all coordinated packages", async () => {
  await mkdir(tmpdir(), { recursive: true });
  const parent = await mkdtemp(join(tmpdir(), "libmorpheus-npm-bootstrap-"));
  const output = join(parent, "packages");
  try {
    const staged = await stageNpmBootstrapPackages(output);
    assert.deepEqual(
      staged.map((item) => item.name),
      [
        "@libmorpheus/node-darwin-arm64",
        "@libmorpheus/node-linux-arm64-gnu",
        "@libmorpheus/node-linux-x64-gnu",
        "@libmorpheus/node",
      ],
    );
    for (const item of staged) {
      const metadata = JSON.parse(
        await readFile(join(item.directory, "package.json"), "utf8"),
      );
      assert.equal(metadata.name, item.name);
      assert.equal(metadata.version, "0.0.0");
      assert.equal(metadata.license, "AGPL-3.0-or-later");
      assert.deepEqual(
        (await readdir(item.directory)).sort(),
        ["LICENSE", "NOTICE", "README.md", "package.json"],
      );
      assert.match(
        await readFile(join(item.directory, "README.md"), "utf8"),
        /contains no executable code/,
      );
    }
    await assert.rejects(
      stageNpmBootstrapPackages(output),
      (error) => error?.code === "EEXIST",
    );
  } finally {
    await rm(parent, { recursive: true, force: true });
  }
});
