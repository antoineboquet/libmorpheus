<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Release decision: 0.3.1

Status: benchmark accepted; platform qualification pending.

- Project version: **0.3.1**
- C ABI: **2**
- SONAME major: **1**
- Previous release tag: `v0.3.0` at `6e13d6ec720529f724b41af60f1394dad1f1c2dc`
- Benchmark evidence: **accepted**

## Version and ABI rationale

Version 0.3.1 is a patch release for the Deno/JSR distribution and its release
qualification. The diff from `v0.3.0` does not change `src/`,
`include/morpheus/morpheus.h`, or `cmake/PublicApi.cmake`; it therefore removes
no public function, changes no signature or record layout, and reassigns no
constant. C ABI 2 and SONAME 1 remain correct.

The native archives are nevertheless rebuilt and republished under 0.3.1 so
the current single project version continues to identify the native library,
standalone Deno archive, JSR package, and benchmark evidence consistently. If
additional bindings are maintained later, their package versions should be
decoupled from the native project version and tied to an explicit ABI
compatibility range.

The generation surface and Deno `generate()`/`generateRaw()` methods remain
experimental until sufficient real-world use complements their automated
differential, isolation, failure, portability, and sanitizer coverage.

## Candidate scope

- Correct the bundled Emscripten preparer so `/data --with-gener` works when
  the module is loaded from its published HTTPS JSR URL.
- Permit the fresh-package smoke test to install an exact newly published
  version despite Deno's default minimum dependency age.
- Add `/setup`, a permission-scoped command that acquires the matching verified
  native archive and selected stem dataset in one transaction-like operation.
- Retain `/native` and `/data` for separate acquisition, including Alpheios
  generation-index preparation and Perseids Greek/Latin analysis data.
- Qualify the published package from an empty Deno application against public
  JSR, GitHub Release, and upstream dataset endpoints.
- Improve package discovery and JSR scoring through entrypoint and exported
  symbol documentation, and correct the README notice link.
- Retain the narrow `v0.3.0` JSR recovery workflow as historical release
  machinery; normal 0.3.1 publication remains part of the tag workflow.

## Licensing boundary

The Deno binding, acquisition orchestration, tests, workflows, and
documentation remain AGPL-3.0-or-later. The inherited generator, translation
bridge, generation index reader, and bundled Emscripten preparer remain
MPL-2.0; the Emscripten runtime portions retain their MIT license. The JSR
`license` field names the package-level AGPL license, while `NOTICE` and the
packaged license files preserve the complete component boundary. Native
archives and stem datasets remain separate distributions.

## Distribution contract

The `v0.3.1` tag is expected to rebuild and publish data-free native archives
and SHA-256 files for Linux x86-64 glibc, Linux aarch64 glibc, and macOS arm64,
plus `libmorpheus-deno-0.3.1.tar.gz`, its sidecar, and the accepted versioned
benchmark evidence. Only after those GitHub Release assets exist may the same
workflow publish `@humanities/libmorpheus@0.3.1`.

The JSR package contains source, acquisition commands, the internal WebAssembly
preparer, and licensing documentation. It embeds neither a native library nor
linguistic data nor `gener.index`. `/setup` downloads the matching native asset
and a pinned upstream dataset, verifies both, and can derive the Alpheios index
locally. Container images remain qualification-only artifacts while dataset and
derived-index redistribution terms are unresolved.

## Benchmark evidence

The accepted schema 2 report was produced on Apple Silicon from
`2545574afddce3a0174297cdd393de6435d2b3e7` with Deno 2.9.6, Apple Clang 21,
the pinned Alpheios revision, and the canonical generation-index digest. Its
SHA-256 is
`336db599db9f87ba06cedb7c2e0435125fc9918a62215ddc6988b5612c94ca78`.
The release validator accepts all 13 required configurations and identities;
the embedded comparison reproduces exactly from the accepted 0.3.0 report.

Compared with 0.3.0, analysis throughput improves by approximately 2.1% to
4.9% across one, two, and four FFI contexts. Persistent and cold `cruncher`
throughput improve by 2.1% and 1.4%. Maximal warm generation changes by +0.2%,
+2.2%, and +1.0%; small warm generation varies by -6.6%, +8.6%, and -0.2%.
Both cold generation workloads improve by approximately 1.1% to 1.2%. Peak
RSS is lower for every measured warm configuration; the maximal cold sample is
9.9% higher, but its process RSS growth is lower and cold samples share the
runner's accumulated high-water mark. These variations are accepted as normal
measurement noise for unchanged native execution paths.

The evidence-finalization commit changes only the benchmark, release metadata,
validation workflow, and documentation after the measured revision. It does
not change the native or Deno runtime implementation, stemlib inputs, corpus,
or generation index.

## Remaining gates

1. Require the complete Linux CI, including Deno package checks, sanitizers,
   signedness, fixtures, and release metadata, to pass.
2. Manually dispatch `Platform and release qualification` for the exact
   benchmark-finalized commit with package artifacts enabled; inspect every
   native and standalone Deno archive and checksum.
3. Tag that qualified commit as `v0.3.1` only after explicit authorization.
   Require both tag workflows to rebuild and publish all versioned assets and
   the JSR package.
4. Manually dispatch `Published JSR smoke test` for `0.3.1` and require Greek
   and Latin analysis plus experimental Greek generation to pass.
