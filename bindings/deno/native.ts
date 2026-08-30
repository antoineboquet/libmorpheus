// SPDX-License-Identifier: AGPL-3.0-or-later

/**
 * Permission-scoped acquisition of a qualified libmorpheus native release.
 *
 * Run this module with `deno x` to install the library selected for the current
 * platform, or use its functions from an application.
 *
 * @example Acquire the native release for the current platform
 * ```sh
 * deno x --allow-net=github.com,release-assets.githubusercontent.com \
 *   --allow-read=./morpheus-native --allow-write=./morpheus-native \
 *   jsr:@humanities/libmorpheus/native --output ./morpheus-native
 * ```
 *
 * @module
 */

import { join, resolve } from "node:path";
import {
  type AcquireMorpheusNativeOptions,
  acquireMorpheusNativeWithDependencies,
  defaultNativeAcquisitionDependencies,
  type MorpheusNativeReceipt,
} from "./internal/native_internal.ts";
import { selectMorpheusNativeTarget } from "./internal/native_manifest.ts";

export {
  MORPHEUS_NATIVE_ABI_VERSION,
  MORPHEUS_NATIVE_VERSION,
} from "./internal/native_manifest.ts";

/** Options and receipt returned by native-library acquisition. */
export type { AcquireMorpheusNativeOptions, MorpheusNativeReceipt };

/** Downloads, verifies, and extracts the native library for this platform. */
export async function acquireMorpheusNative(
  options: AcquireMorpheusNativeOptions,
): Promise<MorpheusNativeReceipt> {
  return await acquireMorpheusNativeWithDependencies(
    options,
    defaultNativeAcquisitionDependencies(),
  );
}

/** Resolves the platform-specific shared-library path below an installed root. */
export function nativeLibraryPath(root: string): string {
  const target = selectMorpheusNativeTarget(Deno.build.os, Deno.build.arch);
  return join(resolve(root), ...target.libraryPath.split("/"));
}

/** Parsed options for the executable `/native` entrypoint. */
export interface MorpheusNativeCommandLineOptions
  extends AcquireMorpheusNativeOptions {
  /** Whether help was requested instead of acquisition. */
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

/** Parses command-line arguments accepted by the `/native` entrypoint. */
export function parseMorpheusNativeArgs(
  args: readonly string[],
): MorpheusNativeCommandLineOptions {
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
