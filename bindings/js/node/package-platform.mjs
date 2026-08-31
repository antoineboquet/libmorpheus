// SPDX-License-Identifier: AGPL-3.0-or-later

import { cp, mkdir, readFile, writeFile } from "node:fs/promises";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const bindingDirectory = dirname(fileURLToPath(import.meta.url));
const targets = new Set([
  "darwin-arm64",
  "linux-arm64-gnu",
  "linux-x64-gnu",
]);

const [target, addonArgument, outputArgument] = process.argv.slice(2);
if (!targets.has(target) || addonArgument === undefined ||
    outputArgument === undefined) {
  throw new Error(
    "usage: node package-platform.mjs <target> <addon.node> <output>",
  );
}

const addon = resolve(addonArgument);
if (basename(addon) !== "libmorpheus_node.node") {
  throw new Error("the addon must be named libmorpheus_node.node");
}
const output = resolve(outputArgument);
const mainPackage = JSON.parse(
  await readFile(join(bindingDirectory, "package.json"), "utf8"),
);
const template = JSON.parse(
  await readFile(
    join(bindingDirectory, "npm", target, "package.json"),
    "utf8",
  ),
);
if (template.version !== mainPackage.version ||
    mainPackage.optionalDependencies?.[template.name] !== template.version) {
  throw new Error(`${template.name} is not aligned with the main package`);
}

await mkdir(output, { recursive: false });
await Promise.all([
  cp(addon, join(output, "libmorpheus_node.node")),
  cp(join(bindingDirectory, "LICENSE"), join(output, "LICENSE")),
  cp(join(bindingDirectory, "NOTICE"), join(output, "NOTICE")),
  writeFile(
    join(output, "package.json"),
    `${JSON.stringify(template, null, 2)}\n`,
    { flag: "wx" },
  ),
]);

console.log(`${template.name}@${template.version} staged in ${output}`);
