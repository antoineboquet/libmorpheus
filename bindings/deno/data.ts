// SPDX-License-Identifier: AGPL-3.0-or-later

/**
 * Permission-scoped acquisition of verified Alpheios or Perseids stem data.
 *
 * Run this module with `deno x` to create a new data directory, or call
 * {@link acquireMorpheusData} from an application that grants the documented
 * network, read, and write permissions.
 *
 * @example Acquire Greek and Latin analysis data
 * ```sh
 * deno x --allow-net=codeload.github.com \
 *   --allow-read=./morpheus-data --allow-write=./morpheus-data \
 *   jsr:@humanities/libmorpheus/data \
 *   --dataset perseids --output ./morpheus-data
 * ```
 *
 * @module
 */

import {
  acquireMorpheusData,
  type AcquireMorpheusDataOptions,
  type MorpheusDataReceipt,
} from "./internal/data_internal.ts";
import type { MorpheusDatasetName } from "./internal/data_manifest.ts";

/** Options, receipt, and dataset names used by data acquisition. */
export type {
  AcquireMorpheusDataOptions,
  MorpheusDataReceipt,
  MorpheusDatasetName,
};
/** Downloads and verifies a pinned stem dataset in a new directory. */
export { acquireMorpheusData };

/** Parsed options for the executable `/data` entrypoint. */
export interface MorpheusDataCommandLineOptions
  extends AcquireMorpheusDataOptions {
  /** Whether help was requested instead of acquisition. */
  readonly help: boolean;
}

function usage(): string {
  return `Usage:
  deno x [permissions] jsr:@humanities/libmorpheus/data \\
    --dataset <alpheios|perseids> --output <directory> [--with-gener]

Datasets:
  alpheios  Ancient Greek analysis; supports experimental --with-gener
  perseids  Ancient Greek and Latin analysis

The command downloads an immutable upstream revision, verifies every selected
file, preserves the upstream license, and refuses to overwrite its output.
`;
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

/** Parses command-line arguments accepted by the `/data` entrypoint. */
export function parseMorpheusDataArgs(
  args: readonly string[],
): MorpheusDataCommandLineOptions {
  let dataset: MorpheusDatasetName | undefined;
  let output: string | undefined;
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
    } else if (argument === "--output") {
      output = takeValue(args, index, "--output");
      index++;
    } else if (argument.startsWith("--output=")) {
      output = argument.slice("--output=".length);
    } else {
      throw new TypeError(`unknown argument: ${argument}`);
    }
  }
  if (help) {
    return {
      dataset: dataset ?? "alpheios",
      output: output ?? ".",
      withGener,
      help,
    };
  }
  if (dataset === undefined) throw new TypeError("--dataset is required");
  if (output === undefined || output === "") {
    throw new TypeError("--output is required");
  }
  if (withGener && dataset !== "alpheios") {
    throw new TypeError("--with-gener is supported only for alpheios");
  }
  return { dataset, output, withGener, help };
}

if (import.meta.main) {
  try {
    const options = parseMorpheusDataArgs(Deno.args);
    if (options.help) {
      console.log(usage());
    } else {
      const receipt = await acquireMorpheusData(options);
      console.log(
        `Prepared ${receipt.dataset} data in ${options.output} ` +
          `(${receipt.files.count} verified files).`,
      );
    }
  } catch (error) {
    console.error(error instanceof Error ? error.message : String(error));
    console.error(usage());
    Deno.exitCode = 1;
  }
}
