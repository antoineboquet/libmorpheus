// SPDX-License-Identifier: AGPL-3.0-or-later

/**
 * Installation guidance for the separately distributed libmorpheus native
 * library and stem data.
 *
 * Calling {@link init} only prints and returns commands: it does not request
 * permissions, access the network, write files, or start subprocesses.
 *
 * @module
 */

import { MORPHEUS_PACKAGE_VERSION } from "./data_manifest.ts";

/** A stem dataset supported by the installation guide. */
export type MorpheusInitDataset = "alpheios" | "perseids";

/** Options used to build an installation plan. */
export interface MorpheusInitOptions {
  /**
   * Stem dataset to acquire. Alpheios is Greek-only; Perseids is Greek and
   * Latin.
   */
  readonly dataset?: MorpheusInitDataset;
  /**
   * Whether to build the experimental Greek generation index.
   * Defaults to `true` for Alpheios and must be `false` for Perseids.
   */
  readonly withGener?: boolean;
  /** Destination for the platform-native library. */
  readonly nativeOutput?: string;
  /** Destination for the selected stem data. */
  readonly dataOutput?: string;
  /** Receives the formatted guide. Defaults to `console.log`. */
  readonly write?: (guide: string) => void;
}

/** Commands and resolved choices produced by {@link init}. */
export interface MorpheusInitPlan {
  /** Package version used in every command. */
  readonly packageVersion: string;
  /** Selected stem dataset. */
  readonly dataset: MorpheusInitDataset;
  /** Whether the data command builds the experimental generation index. */
  readonly withGener: boolean;
  /** Command that records the JSR dependency in a Deno project. */
  readonly packageCommand: string;
  /** Permission-scoped command that acquires the native library. */
  readonly nativeCommand: string;
  /** Permission-scoped command that acquires and verifies stem data. */
  readonly dataCommand: string;
}

function shellArgument(value: string): string {
  return `'${value.replaceAll("'", `'"'"'`)}'`;
}

function nonEmptyPath(value: string, name: string): string {
  if (value.trim() === "") throw new TypeError(`${name} must not be empty`);
  return value;
}

function formatPlan(plan: MorpheusInitPlan): string {
  const generation = plan.withGener
    ? " The data command also builds the experimental Greek generation index."
    : "";
  return `libmorpheus requires three separately installed parts:

1. Add the JSR binding to your project:

${plan.packageCommand}

2. Acquire the matching native library:

${plan.nativeCommand}

3. Acquire the ${plan.dataset} stem data:${generation}

${plan.dataCommand}

Run your application with --allow-ffi. Pass the data output directory to
MorpheusLibrary.createContext() and resolve the native library with
nativeLibraryPath() from @humanities/libmorpheus/native.`;
}

/**
 * Prints and returns the commands required to install the binding, native
 * library, and stem data.
 *
 * This helper is deliberately permission-free and has no installation side
 * effects. Copy and review the returned commands before running them.
 *
 * @example Greek analysis and experimental generation
 * ```ts
 * import { init } from "@humanities/libmorpheus";
 * init();
 * ```
 *
 * @example Greek and Latin analysis
 * ```ts
 * import { init } from "@humanities/libmorpheus/init";
 * init({ dataset: "perseids" });
 * ```
 */
export function init(options: MorpheusInitOptions = {}): MorpheusInitPlan {
  const dataset = options.dataset ?? "alpheios";
  if (dataset !== "alpheios" && dataset !== "perseids") {
    throw new TypeError(`unsupported dataset: ${dataset}`);
  }
  const withGener = options.withGener ?? dataset === "alpheios";
  if (dataset === "perseids" && withGener) {
    throw new TypeError(
      "the experimental generation index is available only with the " +
        "Alpheios dataset",
    );
  }
  const nativeOutput = nonEmptyPath(
    options.nativeOutput ?? "./morpheus-native",
    "nativeOutput",
  );
  const dataOutput = nonEmptyPath(
    options.dataOutput ?? "./morpheus-data",
    "dataOutput",
  );
  const specifier = `jsr:@humanities/libmorpheus@${MORPHEUS_PACKAGE_VERSION}`;
  const nativeArgument = shellArgument(nativeOutput);
  const dataArgument = shellArgument(dataOutput);
  const plan: MorpheusInitPlan = {
    packageVersion: MORPHEUS_PACKAGE_VERSION,
    dataset,
    withGener,
    packageCommand: `deno add ${specifier}`,
    nativeCommand: `deno x \\
  --allow-net=github.com,release-assets.githubusercontent.com \\
  --allow-read=${nativeArgument} \\
  --allow-write=${nativeArgument} \\
  ${specifier}/native \\
  --output ${nativeArgument}`,
    dataCommand: `deno x \\
  --allow-net=codeload.github.com \\
  --allow-read=${dataArgument} \\
  --allow-write=${dataArgument} \\
  ${specifier}/data \\
  --dataset ${dataset}${withGener ? " \\\n  --with-gener" : ""} \\
  --output ${dataArgument}`,
  };
  (options.write ?? console.log)(formatPlan(plan));
  return plan;
}
