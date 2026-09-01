#!/usr/bin/env node
// SPDX-License-Identifier: AGPL-3.0-or-later

import { pathToFileURL } from "node:url";
import { acquireMorpheusData } from "./internal/data-internal.js";

export { acquireMorpheusData };

function takeValue(args, index, name) {
  const value = args[index + 1];
  if (value === undefined || value.startsWith("--")) throw new TypeError(`${name} requires a value`);
  return value;
}

export function parseMorpheusDataArgs(args) {
  let dataset;
  let output;
  let withGener = false;
  let help = false;
  for (let index = 0; index < args.length; index++) {
    const argument = args[index];
    if (argument === "--help" || argument === "-h") help = true;
    else if (argument === "--dataset") {
      dataset = takeValue(args, index, "--dataset");
      index++;
    } else if (argument.startsWith("--dataset=")) dataset = argument.slice("--dataset=".length);
    else if (argument === "--output") {
      output = takeValue(args, index, "--output");
      index++;
    } else if (argument.startsWith("--output=")) output = argument.slice("--output=".length);
    else if (argument === "--with-gener") withGener = true;
    else throw new TypeError(`unknown argument: ${argument}`);
  }
  if (help) return { dataset: dataset ?? "alpheios", output: output ?? ".", withGener, help };
  if (dataset !== "alpheios" && dataset !== "perseids") throw new TypeError(dataset === undefined ? "--dataset is required" : `unsupported dataset: ${dataset}`);
  if (output === undefined || output === "") throw new TypeError("--output is required");
  if (withGener && dataset !== "alpheios") {
    throw new TypeError("--with-gener is supported only for alpheios");
  }
  return { dataset, output, withGener, help };
}

function usage() {
  return `Usage: libmorpheus-data --dataset <alpheios|perseids> --output <directory> [--with-gener]\n\nAlpheios provides Greek analysis and experimental generation; Perseids provides Greek and Latin analysis.`;
}

if (process.argv[1] !== undefined && import.meta.url === pathToFileURL(process.argv[1]).href) {
  try {
    const options = parseMorpheusDataArgs(process.argv.slice(2));
    if (options.help) console.log(usage());
    else {
      const receipt = await acquireMorpheusData(options);
      console.log(`Prepared ${receipt.dataset} data in ${options.output} (${receipt.files.count} verified files).`);
    }
  } catch (error) {
    console.error(error instanceof Error ? error.message : String(error));
    console.error(usage());
    process.exitCode = 1;
  }
}
