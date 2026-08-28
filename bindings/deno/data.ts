// SPDX-License-Identifier: AGPL-3.0-or-later

import {
  acquireMorpheusData,
  type AcquireMorpheusDataOptions,
  type MorpheusDataReceipt,
} from "./data_internal.ts";
import type { MorpheusDatasetName } from "./data_manifest.ts";

export type {
  AcquireMorpheusDataOptions,
  MorpheusDataReceipt,
  MorpheusDatasetName,
};
export { acquireMorpheusData };

interface CommandLineOptions extends AcquireMorpheusDataOptions {
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

export function parseMorpheusDataArgs(
  args: readonly string[],
): CommandLineOptions {
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
