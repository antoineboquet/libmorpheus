<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Release decision: 0.3.0

Status: candidate prepared for final qualification and publication.

- Project version: **0.3.0**
- C ABI: **2**
- SONAME major: **1**
- Previous release tag: `v0.2.0` at `5c071548a7ceea05340dffc60591567cff7ca6c5`

## Version and ABI rationale

Version 0.3.0 is a compatible feature release. It adds Greek lemma generation, a
generation-options structure, an owned generation-result type, generation
accessors, and a result-limit status. It does not remove a public function,
change a published signature, reassign an existing constant, or change the
layout of `morpheus_analysis`. ABI 2 and SONAME 1 therefore remain correct.

The new C generation surface and Deno `generate()`/`generateRaw()` methods are
experimental. That maturity label does not weaken the ownership, symbol, or
binary-compatibility contracts of the additive ABI 2 declarations.

## Candidate scope

- Build a deterministic, validated, duplicate-preserving reverse generation
  index from the pinned Greek stem sources.
- Expand derivations and continuations offline without reviving the historical
  `gener` and `do_conj` request paths.
- Generate through a context-local internal service that invokes only
  `GenStemForms()` and `GenIrregForm()`.
- Normalize generated interpretations at the MPL/AGPL bridge, preserving
  dialects, duals, duplicates, stable ordering, and explicit result limits.
- Publish the additive C API and nonblocking typed Deno binding.
- Qualify cold and warm generation, small and maximal paradigms, concurrency,
  and memory behavior through benchmark schema 2.
- Improve the standalone Deno and repository documentation, including Docker,
  stemlib provenance, FFI permissions, and the experimental maturity notice.

## Licensing boundary

The inherited generator, source-format translation, generation service,
normalizer, derivation expansion, and historical compatibility expressions
remain MPL-2.0. The independently written public API orchestration, Deno
binding, tests, build and release support, and documentation remain
AGPL-3.0-or-later. Generated index data follows the licensing and redistribution
terms of its stemlib inputs.

## Distribution contract

The candidate continues to produce data-free native archives and SHA-256 files
for Linux x86-64 glibc, Linux aarch64 glibc, and macOS arm64, plus the
platform-independent `libmorpheus-deno-0.3.0.tar.gz` source archive. Native
binaries and stem data are not embedded in the Deno archive.

The Deno source package is configured for JSR under the reserved name
`@humanities/libmorpheus` at candidate version 0.3.0. Its metadata, exact
source-only contents, import command, and strict registry dry-run are covered by
the ordinary Linux CI. It includes the public `/data` and `/native` commands and
their internal support, but no downloaded binary or linguistic data. The JSR
package retains the AGPL notice and the documented MPL boundary to the
separately distributed native runtime. The `/native` command verifies and
extracts the matching release asset without a C toolchain.

Container builds remain qualification artifacts. They embed the pinned Alpheios
stemlib and its validated derived `gener.index` so Docker qualification covers
real experimental generation. Such an image must not be published until the
dataset and derived-index redistribution terms have been confirmed.

## Benchmark evidence

The accepted schema 2 report was produced on Apple Silicon from
`66e656d27aedce04cc7b96fe84aff3aca8a35afd` with Deno 2.9.5, Apple Clang 21,
the pinned Alpheios revision, and the canonical generation-index digest. Its
SHA-256 is
`82875bed7e33312a1315819dc8c73fb53aa79a05c8aef0407b5768c2baf2a290`.
The release validator accepts every required measurement and identity.

The commits between that measured revision and this finalization change only
JSR data/native acquisition, the opt-in Emscripten preparer build, Docker and
release orchestration, licensing, and documentation. They do not change
`src/`, `include/`, `bindings/js/deno/mod.ts`, `bench/compare.ts`, or
`test/fixture.json`; the measured analysis and generation paths are therefore
unchanged. The report is retained as preparation evidence rather than described
as tag-exact evidence.

Compared with 0.2.0, analysis throughput changes by approximately -1.2%, -1.5%
and +0.6% for one, two and four FFI contexts. Persistent `cruncher` throughput
changes by +0.6% and cold startup throughput by -2.2%. Peak FFI RSS increases by
approximately 6.6% to 7.5%, while measured RSS growth decreases. These small
changes are accepted. Schema 2 establishes the initial generation baseline:
warm `lo/gos` generation scales from about 8,006 to 22,965 operations/second,
while the maximal `i(/hmi` workload scales from about 132 to 175
operations/second across one to four contexts.

## Remaining gates

1. Manually dispatch the architecture workflow for the exact finalization commit
   with package artifacts enabled and inspect every archive and checksum.
2. Confirm Linux x86-64, Linux aarch64, Alpine x86-64/aarch64, macOS arm64,
   ASan/UBSan, ThreadSanitizer, signedness, Deno FFI, and inherited Makefile
   qualification.
3. Tag the qualified commit as `v0.3.0` and require both tag workflows to pass.
4. Publish only tag-produced archives and the validated benchmark evidence. The
   tagged platform workflow publishes the native and standalone Deno assets
   after tagged Linux CI succeeds, then publishes JSR from the same qualified
   revision. The JSR package must first be linked to this GitHub repository in
   its JSR settings.
