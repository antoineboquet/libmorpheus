// SPDX-License-Identifier: AGPL-3.0-or-later

/**
 * Combined acquisition of the native library and verified stem data.
 *
 * This executable entrypoint provides the second installation step after
 * `deno add jsr:@humanities/libmorpheus`. It requires explicitly scoped
 * network, read, and write permissions.
 *
 * @example Install Greek data and the experimental generation index
 * ```sh
 * deno x \
 *   --allow-net=github.com,release-assets.githubusercontent.com,codeload.github.com \
 *   --allow-read=./morpheus-native,./morpheus-data \
 *   --allow-write=./morpheus-native,./morpheus-data \
 *   jsr:@humanities/libmorpheus/setup \
 *   --dataset alpheios --with-gener
 * ```
 *
 * @module
 */

import { acquireMorpheusData } from "./data_internal.ts";
import type { MorpheusDatasetName } from "./data_manifest.ts";
import {
  acquireMorpheusNativeWithDependencies,
  defaultNativeAcquisitionDependencies,
} from "./native_internal.ts";
import {
  type MorpheusSetupOptions,
  type MorpheusSetupReceipt,
  setupMorpheusWithDependencies,
} from "./setup_internal.ts";

/** Options and receipt returned by combined setup. */
export type { MorpheusSetupOptions, MorpheusSetupReceipt };

/**
 * Installs the matching native library and selected stem data.
 *
 * Both output paths must be absent, separate, and non-overlapping. If data
 * acquisition fails after the native installation succeeds, the newly created
 * native directory is removed.
 */
export async function setupMorpheus(
  options: MorpheusSetupOptions,
): Promise<MorpheusSetupReceipt> {
  const nativeDependencies = defaultNativeAcquisitionDependencies();
  return await setupMorpheusWithDependencies(options, {
    pathExists: async (path) => {
      try {
        await Deno.lstat(path);
        return true;
      } catch (error) {
        if (error instanceof Deno.errors.NotFound) return false;
        throw error;
      }
    },
    remove: (path, removeOptions) => Deno.remove(path, removeOptions),
    acquireNative: (nativeOptions) =>
      acquireMorpheusNativeWithDependencies(
        nativeOptions,
        nativeDependencies,
      ),
    acquireData: acquireMorpheusData,
  });
}

/** Parsed options for the executable `/setup` entrypoint. */
export interface MorpheusSetupCommandLineOptions extends MorpheusSetupOptions {
  /** Whether help was requested instead of installation. */
  readonly help: boolean;
}

function takeValue(
  args: readonly string[],
  index: number,
  name: string,
): string {
  const value = args[index + 1];
  if (value === undefined || value.startsWith("--")) {
    throw new TypeError(`${name} requires a value`);
  }
  return value;
}

/** Parses command-line arguments accepted by the `/setup` entrypoint. */
export function parseMorpheusSetupArgs(
  args: readonly string[],
): MorpheusSetupCommandLineOptions {
  let dataset: MorpheusDatasetName | undefined;
  let nativeOutput = "./morpheus-native";
  let dataOutput = "./morpheus-data";
  let withGener = false;
  let help = false;
  for (let index = 0; index < args.length; index++) {
    const argument = args[index];
    if (argument === "--help" || argument === "-h") {
      help = true;
    } else if (argument === "--with-gener") {
      withGener = true;
    } else if (argument === "--dataset") {
      const value = takeValue(args, index, "--dataset");
      if (value !== "alpheios" && value !== "perseids") {
        throw new TypeError(`unsupported dataset: ${value}`);
      }
      dataset = value;
      index++;
    } else if (argument.startsWith("--dataset=")) {
      const value = argument.slice("--dataset=".length);
      if (value !== "alpheios" && value !== "perseids") {
        throw new TypeError(`unsupported dataset: ${value}`);
      }
      dataset = value;
    } else if (argument === "--native-output") {
      nativeOutput = takeValue(args, index, "--native-output");
      index++;
    } else if (argument.startsWith("--native-output=")) {
      nativeOutput = argument.slice("--native-output=".length);
    } else if (argument === "--data-output") {
      dataOutput = takeValue(args, index, "--data-output");
      index++;
    } else if (argument.startsWith("--data-output=")) {
      dataOutput = argument.slice("--data-output=".length);
    } else {
      throw new TypeError(`unknown argument: ${argument}`);
    }
  }
  if (help) {
    return {
      dataset: dataset ?? "alpheios",
      nativeOutput,
      dataOutput,
      withGener,
      help,
    };
  }
  if (dataset === undefined) throw new TypeError("--dataset is required");
  if (withGener && dataset !== "alpheios") {
    throw new TypeError("--with-gener is supported only with alpheios");
  }
  return { dataset, nativeOutput, dataOutput, withGener, help };
}

function usage(): string {
  return `Usage:
  deno x [permissions] jsr:@humanities/libmorpheus/setup \\
    --dataset <alpheios|perseids> [--with-gener] \\
    [--native-output <directory>] [--data-output <directory>]

The command installs the matching native library and verified stem data.
Alpheios provides Greek analysis and experimental generation; Perseids provides
Greek and Latin analysis. Output directories must be absent and non-overlapping.
`;
}

if (import.meta.main) {
  try {
    const options = parseMorpheusSetupArgs(Deno.args);
    if (options.help) {
      console.log(usage());
    } else {
      const receipt = await setupMorpheus(options);
      console.log(`Installed native library: ${receipt.nativeLibraryPath}`);
      console.log(`Installed stem data: ${receipt.dataOutput}`);
      console.log("Run applications with Deno's --allow-ffi permission.");
    }
  } catch (error) {
    console.error(error instanceof Error ? error.message : String(error));
    console.error(usage());
    Deno.exitCode = 1;
  }
}
