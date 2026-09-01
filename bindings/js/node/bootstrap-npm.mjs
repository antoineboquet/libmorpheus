#!/usr/bin/env node
// SPDX-License-Identifier: AGPL-3.0-or-later

import { cp, mkdir, writeFile } from "node:fs/promises";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

const bindingDirectory = dirname(fileURLToPath(import.meta.url));
const packageNames = [
  "@libmorpheus/node-darwin-arm64",
  "@libmorpheus/node-linux-arm64-gnu",
  "@libmorpheus/node-linux-x64-gnu",
  "@libmorpheus/node",
];

function directoryName(packageName) {
  return packageName.slice("@libmorpheus/".length);
}

export async function stageNpmBootstrapPackages(outputArgument) {
  if (outputArgument === undefined || outputArgument === "") {
    throw new TypeError("an output directory is required");
  }
  const output = resolve(outputArgument);
  await mkdir(output, { recursive: false });
  const staged = [];
  for (const name of packageNames) {
    const directory = join(output, directoryName(name));
    await mkdir(directory);
    const packageJson = {
      name,
      version: "0.0.0",
      description: "Reserved for the official libmorpheus Node.js binding",
      license: "AGPL-3.0-or-later",
      repository: {
        type: "git",
        url: "https://github.com/defense-humanites/libmorpheus.git",
        directory: "bindings/js/node",
      },
      publishConfig: { access: "public" },
      files: ["LICENSE", "NOTICE", "README.md"],
    };
    await Promise.all([
      cp(join(bindingDirectory, "LICENSE"), join(directory, "LICENSE")),
      cp(join(bindingDirectory, "NOTICE"), join(directory, "NOTICE")),
      writeFile(
        join(directory, "package.json"),
        `${JSON.stringify(packageJson, null, 2)}\n`,
        { flag: "wx" },
      ),
      writeFile(
        join(directory, "README.md"),
        `# ${name}\n\n` +
          "Version 0.0.0 only reserves this official package name so npm " +
          "trusted publishing can be configured. Install a later version; " +
          "this placeholder contains no executable code.\n",
        { flag: "wx" },
      ),
    ]);
    staged.push({ directory, name, version: "0.0.0" });
  }
  return staged;
}

function parseArgs(args) {
  if (args.length === 2 && args[0] === "--output" && args[1] !== "") {
    return args[1];
  }
  throw new TypeError("usage: node bootstrap-npm.mjs --output <directory>");
}

if (process.argv[1] !== undefined &&
    import.meta.url === pathToFileURL(process.argv[1]).href) {
  try {
    const staged = await stageNpmBootstrapPackages(
      parseArgs(process.argv.slice(2)),
    );
    for (const item of staged) {
      console.log(`${item.name}@${item.version}: ${item.directory}`);
    }
  } catch (error) {
    console.error(error instanceof Error ? error.message : String(error));
    process.exitCode = 1;
  }
}
