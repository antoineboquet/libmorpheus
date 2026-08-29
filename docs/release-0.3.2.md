<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Release decision: 0.3.2

Status: benchmark accepted; platform qualification pending.

- Project version: **0.3.2**
- C ABI: **2**
- SONAME major: **1**
- Previous release tag: `v0.3.1` at `c899e51aafbbf22f98b88a22f0d0a6d247d91850`
- Benchmark evidence: **accepted**

## Version and ABI rationale

Version 0.3.2 is a patch release correcting the Deno/JSR native installer. The
diff from `v0.3.1` does not change `src/`, `include/morpheus/morpheus.h`, or
`cmake/PublicApi.cmake`; it removes no public function, changes no signature or
record layout, and reassigns no constant. C ABI 2 and SONAME 1 remain correct.

The current project still coordinates one version across native archives, the
standalone Deno archive, benchmark evidence, and JSR. All assets are therefore
rebuilt as 0.3.2. Decoupling future binding versions from the native project and
expressing an explicit compatible ABI range remains a separate architectural
change for a repository maintaining multiple bindings.

The generation surface and Deno `generate()`/`generateRaw()` methods remain
experimental until sufficient real-world use complements their automated
differential, isolation, failure, portability, and sanitizer coverage.

## Failure and correction

The published 0.3.1 `/native` and `/setup` commands verified the native archive
correctly, but attempted to recreate its unversioned and SONAME aliases with
`Deno.symlink()`. Deno requires unscoped read and write permissions for that
operation, so the documented path-scoped command failed before installing the
archive.

The 0.3.2 extractor validates the same archive paths and link targets, resolves
link chains entirely within the verified in-memory archive, rejects missing,
non-regular, escaping, cyclic, and duplicate entries, and materializes each
shared-library alias as a regular copy. The unversioned, SONAME, and fully
versioned paths therefore remain loadable without granting filesystem-wide
permissions.

CI reproduces the real two-link Linux archive structure and runs that test with
only path-scoped `--allow-read` and `--allow-write`. Existing traversal,
redirect, checksum, cleanup, duplicate, and output-isolation tests remain in
force.

## Licensing boundary

The corrected Deno acquisition orchestration, tests, workflow, and
documentation remain AGPL-3.0-or-later. No inherited MPL-covered native engine,
bridge, generator, index reader, or Emscripten preparer source changes. Native
archives and stem datasets remain separate distributions with their existing
notices and receipts.

## Distribution contract

The `v0.3.2` tag is expected to rebuild and publish data-free native archives
and SHA-256 files for Linux x86-64 glibc, Linux aarch64 glibc, and macOS arm64,
plus `libmorpheus-deno-0.3.2.tar.gz`, its sidecar, and accepted versioned
benchmark evidence. Only after those GitHub Release assets exist may the same
workflow publish `@humanities/libmorpheus@0.3.2`.

The JSR package continues to embed neither a native library nor stem data nor
`gener.index`. `/setup` downloads the matching 0.3.2 native asset and a pinned
upstream dataset, verifies both, and can derive the experimental Alpheios index
locally. Container images remain qualification-only artifacts while dataset
and derived-index redistribution terms are unresolved.

## Benchmark evidence

The accepted schema 2 report was produced on Apple Silicon from
`81aac2dacf454a692fa83eecd98ade672209027f` with Deno 2.9.6, Apple Clang 21,
the pinned Alpheios revision, and the canonical generation-index digest. Its
SHA-256 is
`5b20eb33ac4f1731bc3b1f1a417afd381d6f9573ad7bcb73b6ae91914b0c83d6`.
The release validator accepts all 13 required configurations and identities;
the embedded comparison reproduces exactly from the accepted 0.3.1 report.

Compared with 0.3.1, analysis throughput changes by approximately -1.2%,
-5.9%, and -1.6% across one, two, and four FFI contexts. Persistent and cold
`cruncher` throughput change by -0.4% and -2.0%. Maximal warm generation changes
by +1.3%, -0.4%, and +2.9%; small warm generation changes by +0.6%, -2.6%, and
-4.4%. Cold generation is effectively unchanged. Peak RSS is lower in every
measured configuration. These variations are accepted as normal measurement
noise for unchanged analysis and generation execution paths.

The evidence-finalization commit changes only the benchmark, release metadata,
validation workflow, and documentation after the measured revision. It does
not change the corrected installer, native or default Deno runtime paths,
stemlib inputs, corpus, or generation index.

## Remaining gates

1. Require the complete Linux CI, including the path-scoped native acquisition
   test, Deno package checks, sanitizers, signedness, fixtures, and release
   metadata, to pass.
2. Manually dispatch `Platform and release qualification` for the exact
   benchmark-finalized commit with package artifacts enabled; inspect every
   native and standalone Deno archive and checksum.
3. Tag that qualified commit as `v0.3.2` only after explicit authorization.
   Require both tag workflows to rebuild and publish all versioned assets and
   the JSR package.
4. Manually dispatch `Published JSR smoke test` for `0.3.2`; require the
   path-scoped combined setup, Greek and Latin analysis, and experimental Greek
   generation to pass.
