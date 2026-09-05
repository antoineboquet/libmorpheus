#!/usr/bin/env node
// SPDX-License-Identifier: AGPL-3.0-or-later

import { join, resolve } from "node:path";

import {
  acquireMorpheusNativeWithDependencies,
  defaultNativeAcquisitionDependencies,
} from "./internal/native-internal.js";
import {
  MORPHEUS_NATIVE_ABI_VERSION,
  MORPHEUS_NATIVE_VERSION,
  selectMorpheusNativeTarget,
} from "./internal/native-manifest.js";
import { isMainModule } from "./internal/main-module.js";

export { MORPHEUS_NATIVE_ABI_VERSION, MORPHEUS_NATIVE_VERSION };

export function acquireMorpheusNative(options) {
  return acquireMorpheusNativeWithDependencies(options, defaultNativeAcquisitionDependencies());
}

export function nativeLibraryPath(root) {
  const target = selectMorpheusNativeTarget(
    process.platform,
    process.arch,
    process.report?.getReport()?.header?.glibcVersionRuntime,
  );
  return join(resolve(root), ...target.libraryPath.split("/"));
}

export function parseMorpheusNativeArgs(args) {
  let output;
  let help = false;
  for (let index = 0; index < args.length; index++) {
    const argument = args[index];
    if (argument === "--help" || argument === "-h") help = true;
    else if (argument === "--output") {
      const value = args[index + 1];
      if (value === undefined || value.startsWith("--")) throw new TypeError("--output requires a value");
      output = value;
      index++;
    } else if (argument.startsWith("--output=")) output = argument.slice("--output=".length);
    else throw new TypeError(`unknown argument: ${argument}`);
  }
  if (help) return { output: output ?? ".", help };
  if (output === undefined || output === "") throw new TypeError("--output is required");
  return { output, help };
}

function usage() {
  return `Usage: libmorpheus-native --output <directory>\n\nDownloads, verifies, and safely extracts libmorpheus. The destination must not exist.`;
}

if (isMainModule(import.meta.url)) {
  try {
    const options = parseMorpheusNativeArgs(process.argv.slice(2));
    if (options.help) console.log(usage());
    else {
      const receipt = await acquireMorpheusNative(options);
      console.log(`Installed ${receipt.target} native runtime in ${options.output}.`);
    }
  } catch (error) {
    console.error(error instanceof Error ? error.message : String(error));
    console.error(usage());
    process.exitCode = 1;
  }
}
