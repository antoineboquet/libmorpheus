// SPDX-License-Identifier: AGPL-3.0-or-later

import { join, resolve } from "node:path";
import {
  type AcquireMorpheusNativeOptions,
  acquireMorpheusNativeWithDependencies,
  defaultNativeAcquisitionDependencies,
  type MorpheusNativeReceipt,
} from "./native_internal.ts";
import { selectMorpheusNativeTarget } from "./native_manifest.ts";

export type { AcquireMorpheusNativeOptions, MorpheusNativeReceipt };

export async function acquireMorpheusNative(
  options: AcquireMorpheusNativeOptions,
): Promise<MorpheusNativeReceipt> {
  return await acquireMorpheusNativeWithDependencies(
    options,
    defaultNativeAcquisitionDependencies(),
  );
}

export function nativeLibraryPath(root: string): string {
  const target = selectMorpheusNativeTarget(Deno.build.os, Deno.build.arch);
  return join(resolve(root), ...target.libraryPath.split("/"));
}

interface CommandLineOptions extends AcquireMorpheusNativeOptions {
  readonly help: boolean;
}

function usage(): string {
  return `Usage:
  deno x [permissions] jsr:@humanities/libmorpheus/native \\
    --output <directory>

The command downloads the matching native GitHub Release archive and its
SHA-256 sidecar, verifies and safely extracts it, writes MORPHEUS-NATIVE.json,
and refuses to overwrite its output. Supported targets are Linux x86_64 glibc,
Linux aarch64 glibc, and macOS arm64.
`;
}

export function parseMorpheusNativeArgs(
  args: readonly string[],
): CommandLineOptions {
  let output: string | undefined;
  let help = false;
  for (let index = 0; index < args.length; index++) {
    const argument = args[index];
    if (argument === "--help" || argument === "-h") {
      help = true;
    } else if (argument === "--output") {
      const value = args[index + 1];
      if (value === undefined || value.startsWith("--")) {
        throw new TypeError("--output requires a value");
      }
      output = value;
      index++;
    } else if (argument.startsWith("--output=")) {
      output = argument.slice("--output=".length);
    } else {
      throw new TypeError(`unknown argument: ${argument}`);
    }
  }
  if (help) return { output: output ?? ".", help };
  if (output === undefined || output === "") {
    throw new TypeError("--output is required");
  }
  return { output, help };
}

if (import.meta.main) {
  try {
    const options = parseMorpheusNativeArgs(Deno.args);
    if (options.help) {
      console.log(usage());
    } else {
      const receipt = await acquireMorpheusNative(options);
      console.log(
        `Installed ${receipt.target} native runtime in ${options.output}.`,
      );
    }
  } catch (error) {
    console.error(error instanceof Error ? error.message : String(error));
    console.error(usage());
    Deno.exitCode = 1;
  }
}
