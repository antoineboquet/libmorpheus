// SPDX-License-Identifier: AGPL-3.0-or-later

import { MORPHEUS_PACKAGE_VERSION } from "./data_manifest.ts";

export interface MorpheusNativeTarget {
  readonly os: "linux" | "darwin";
  readonly arch: "x86_64" | "aarch64";
  readonly name: string;
  readonly asset: string;
  readonly archiveRoot: string;
  readonly libraryPath: string;
}

const targets = [
  {
    os: "linux",
    arch: "x86_64",
    name: "linux-x86_64-glibc",
    asset: `libmorpheus-${MORPHEUS_PACKAGE_VERSION}-Linux-x86_64-glibc.tar.gz`,
    archiveRoot: `libmorpheus-${MORPHEUS_PACKAGE_VERSION}-Linux-x86_64-glibc/`,
    libraryPath: "lib/libmorpheus.so",
  },
  {
    os: "linux",
    arch: "aarch64",
    name: "linux-aarch64-glibc",
    asset: `libmorpheus-${MORPHEUS_PACKAGE_VERSION}-Linux-aarch64-glibc.tar.gz`,
    archiveRoot: `libmorpheus-${MORPHEUS_PACKAGE_VERSION}-Linux-aarch64-glibc/`,
    libraryPath: "lib/libmorpheus.so",
  },
  {
    os: "darwin",
    arch: "aarch64",
    name: "macos-arm64",
    asset: `libmorpheus-${MORPHEUS_PACKAGE_VERSION}-macOS-arm64.tar.gz`,
    archiveRoot: `libmorpheus-${MORPHEUS_PACKAGE_VERSION}-macOS-arm64/`,
    libraryPath: "lib/libmorpheus.dylib",
  },
] as const satisfies readonly MorpheusNativeTarget[];

export const MORPHEUS_NATIVE_SCHEMA_VERSION = 1;
export const MORPHEUS_NATIVE_ABI_VERSION = 2;
export const MORPHEUS_NATIVE_REPOSITORY =
  "https://github.com/defense-humanites/libmorpheus";

export function selectMorpheusNativeTarget(
  os: string,
  arch: string,
): MorpheusNativeTarget {
  const target = targets.find((candidate) =>
    candidate.os === os && candidate.arch === arch
  );
  if (target === undefined) {
    throw new TypeError(`unsupported native platform: ${os}/${arch}`);
  }
  return target;
}
