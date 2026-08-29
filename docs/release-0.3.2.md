<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Release decision: 0.3.2

Status: candidate prepared; benchmark and platform qualification pending.

- Project version: **0.3.2**
- C ABI: **2**
- SONAME major: **1**
- Previous release tag: `v0.3.1` at `c899e51aafbbf22f98b88a22f0d0a6d247d91850`
- Benchmark evidence: **pending**

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

Benchmark evidence is not yet accepted for 0.3.2. Before tagging, run
`bench/release.sh` on the controlled host to create `benchmark-0.3.2.json`,
compare it with the accepted 0.3.1 report, validate its source, stemlib,
compiler, corpus, and generation-index identities, then commit the report and
SHA-256 sidecar under `bench/release-evidence/`.

The native and default Deno analysis and generation implementations are
unchanged from 0.3.1. A material measured difference would therefore be
unexpected and must be investigated rather than attributed to archive
extraction, which is outside the benchmark path.

## Remaining gates

1. Produce, compare, validate, and accept `benchmark-0.3.2.json` and its
   SHA-256 sidecar from the exact candidate revision.
2. Require the complete Linux CI, including the path-scoped native acquisition
   test, Deno package checks, sanitizers, signedness, fixtures, and release
   metadata, to pass.
3. Manually dispatch `Platform and release qualification` for the exact
   benchmark-finalized commit with package artifacts enabled; inspect every
   native and standalone Deno archive and checksum.
4. Tag that qualified commit as `v0.3.2` only after explicit authorization.
   Require both tag workflows to rebuild and publish all versioned assets and
   the JSR package.
5. Manually dispatch `Published JSR smoke test` for `0.3.2`; require the
   path-scoped combined setup, Greek and Latin analysis, and experimental Greek
   generation to pass.
