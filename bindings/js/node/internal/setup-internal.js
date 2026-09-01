// SPDX-License-Identifier: AGPL-3.0-or-later

import { isAbsolute, relative, resolve } from "node:path";

function nonEmptyPath(value, name) {
  if (value.trim() === "") throw new TypeError(`${name} must not be empty`);
  return resolve(value);
}

function containsPath(parent, child) {
  const candidate = relative(parent, child);
  return candidate === "" || (!candidate.startsWith("..") && !isAbsolute(candidate));
}

export async function setupMorpheusWithDependencies(options, dependencies) {
  if (options.dataset !== "alpheios" && options.dataset !== "perseids") {
    throw new TypeError(`unsupported dataset: ${options.dataset}`);
  }
  if (options.withGener && options.dataset !== "alpheios") {
    throw new TypeError("--with-gener is supported only with the Alpheios dataset");
  }
  const nativeOutput = nonEmptyPath(options.nativeOutput ?? "./morpheus-native", "nativeOutput");
  const dataOutput = nonEmptyPath(options.dataOutput ?? "./morpheus-data", "dataOutput");
  if (containsPath(nativeOutput, dataOutput) || containsPath(dataOutput, nativeOutput)) {
    throw new TypeError("native and data outputs must be separate, non-overlapping directories");
  }
  for (const output of [nativeOutput, dataOutput]) {
    if (await dependencies.pathExists(output)) throw new Error(`output path already exists: ${output}`);
  }
  const native = await dependencies.acquireNative({ output: nativeOutput });
  let data;
  try {
    data = await dependencies.acquireData({
      dataset: options.dataset,
      output: dataOutput,
      withGener: options.withGener,
    });
  } catch (error) {
    try {
      await dependencies.remove(nativeOutput, { recursive: true, force: true });
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
