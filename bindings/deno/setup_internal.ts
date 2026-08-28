// SPDX-License-Identifier: AGPL-3.0-or-later

import { isAbsolute, relative, resolve } from "node:path";
import type {
  AcquireMorpheusDataOptions,
  MorpheusDataReceipt,
} from "./data_internal.ts";
import type { MorpheusDatasetName } from "./data_manifest.ts";
import type {
  AcquireMorpheusNativeOptions,
  MorpheusNativeReceipt,
} from "./native_internal.ts";

/** Options for combined native-library and stem-data installation. */
export interface MorpheusSetupOptions {
  /** Stem dataset to install: Alpheios for Greek or Perseids for Greek and Latin. */
  readonly dataset: MorpheusDatasetName;
  /** Build the experimental Greek generation index with Alpheios data. */
  readonly withGener?: boolean;
  /** New native-library directory. Defaults to `./morpheus-native`. */
  readonly nativeOutput?: string;
  /** New stem-data directory. Defaults to `./morpheus-data`. */
  readonly dataOutput?: string;
}

/** Verified results and resolved paths from combined setup. */
export interface MorpheusSetupReceipt {
  /** Native acquisition receipt. */
  readonly native: MorpheusNativeReceipt;
  /** Stem-data acquisition receipt. */
  readonly data: MorpheusDataReceipt;
  /** Absolute native installation directory. */
  readonly nativeOutput: string;
  /** Absolute stem-data installation directory. */
  readonly dataOutput: string;
  /** Absolute shared-library path accepted by the main binding. */
  readonly nativeLibraryPath: string;
}

export interface MorpheusSetupDependencies {
  readonly pathExists: (path: string) => Promise<boolean>;
  readonly remove: (
    path: string,
    options: { readonly recursive: true },
  ) => Promise<void>;
  readonly acquireNative: (
    options: AcquireMorpheusNativeOptions,
  ) => Promise<MorpheusNativeReceipt>;
  readonly acquireData: (
    options: AcquireMorpheusDataOptions,
  ) => Promise<MorpheusDataReceipt>;
}

function nonEmptyPath(value: string, name: string): string {
  if (value.trim() === "") throw new TypeError(`${name} must not be empty`);
  return resolve(value);
}

function containsPath(parent: string, child: string): boolean {
  const candidate = relative(parent, child);
  return candidate === "" ||
    (!candidate.startsWith("..") && !isAbsolute(candidate));
}

export async function setupMorpheusWithDependencies(
  options: MorpheusSetupOptions,
  dependencies: MorpheusSetupDependencies,
): Promise<MorpheusSetupReceipt> {
  if (options.dataset !== "alpheios" && options.dataset !== "perseids") {
    throw new TypeError(`unsupported dataset: ${options.dataset}`);
  }
  if (options.withGener && options.dataset !== "alpheios") {
    throw new TypeError(
      "--with-gener is supported only with the Alpheios dataset",
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
  if (
    containsPath(nativeOutput, dataOutput) ||
    containsPath(dataOutput, nativeOutput)
  ) {
    throw new TypeError(
      "native and data outputs must be separate, non-overlapping directories",
    );
  }
  for (const output of [nativeOutput, dataOutput]) {
    if (await dependencies.pathExists(output)) {
      throw new Error(`output path already exists: ${output}`);
    }
  }

  const native = await dependencies.acquireNative({ output: nativeOutput });
  let data: MorpheusDataReceipt;
  try {
    data = await dependencies.acquireData({
      dataset: options.dataset,
      output: dataOutput,
      withGener: options.withGener,
    });
  } catch (error) {
    try {
      await dependencies.remove(nativeOutput, { recursive: true });
    } catch (cleanupError) {
      throw new AggregateError(
        [error, cleanupError],
        `data acquisition failed and native rollback failed for ${nativeOutput}`,
      );
    }
    throw error;
  }

  return {
    native,
    data,
    nativeOutput,
    dataOutput,
    nativeLibraryPath: resolve(nativeOutput, native.libraryPath),
  };
}
