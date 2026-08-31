// SPDX-License-Identifier: AGPL-3.0-or-later

export type MorpheusDatasetName = "alpheios" | "perseids";
export interface AcquireMorpheusDataOptions {
  readonly dataset: MorpheusDatasetName;
  readonly output: string;
}
export interface MorpheusDataReceipt {
  readonly schema: number;
  readonly packageVersion: string;
  readonly dataset: MorpheusDatasetName;
  readonly languages: readonly string[];
  readonly source: { readonly repository: string; readonly revision: string; readonly archiveUrl: string };
  readonly files: { readonly count: number; readonly treeSha256: string };
  readonly generation: {
    readonly experimental: true;
    readonly available: false;
    readonly indexSha256: null;
    readonly supportSource: null;
  };
}
export interface MorpheusDataCommandLineOptions extends AcquireMorpheusDataOptions { readonly help: boolean }
export function acquireMorpheusData(options: AcquireMorpheusDataOptions): Promise<MorpheusDataReceipt>;
export function parseMorpheusDataArgs(args: readonly string[]): MorpheusDataCommandLineOptions;
