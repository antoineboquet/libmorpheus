// SPDX-License-Identifier: AGPL-3.0-or-later

import type { MorpheusDataReceipt, MorpheusDatasetName } from "./data.js";
import type { MorpheusNativeReceipt } from "./native.js";

export interface MorpheusSetupOptions {
  readonly dataset: MorpheusDatasetName;
  readonly withGener?: boolean;
  readonly nativeOutput?: string;
  readonly dataOutput?: string;
}
export interface MorpheusSetupReceipt {
  readonly native: MorpheusNativeReceipt;
  readonly data: MorpheusDataReceipt;
  readonly nativeOutput: string;
  readonly dataOutput: string;
  readonly nativeLibraryPath: string;
}
export interface MorpheusSetupCommandLineOptions extends MorpheusSetupOptions {
  readonly help: boolean;
  readonly nativeOutput: string;
  readonly dataOutput: string;
}
export function setupMorpheus(options: MorpheusSetupOptions): Promise<MorpheusSetupReceipt>;
export function parseMorpheusSetupArgs(args: readonly string[]): MorpheusSetupCommandLineOptions;
