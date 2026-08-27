<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Release decision: 0.3.0

Status: candidate prepared for final qualification and publication.

- Project version: **0.3.0**
- C ABI: **2**
- SONAME major: **1**
- Previous release tag: `v0.2.0` at
  `5c071548a7ceea05340dffc60591567cff7ca6c5`

## Version and ABI rationale

Version 0.3.0 is a compatible feature release. It adds Greek lemma generation,
a generation-options structure, an owned generation-result type, generation
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

The Deno source package is also intended for JSR under the reserved name
`@humanities/libmorpheus`. Its publication metadata and exact import command
must be qualified before the package is published. The JSR package must retain
the AGPL notice and the documented MPL boundary to the separately distributed
native runtime.

Container builds remain qualification artifacts. An image embedding the pinned
Alpheios stemlib must not be published until that dataset's redistribution terms
have been confirmed.

## Remaining gates

1. Configure and dry-run the `@humanities/libmorpheus` JSR package, then verify
   its exported surface, README, license, notice, and source-only contents.
2. Produce and validate the 0.3.0 benchmark report on the controlled release
   host against the previous accepted report.
3. Manually dispatch the architecture workflow for the exact candidate commit
   with package artifacts enabled and inspect every archive and checksum.
4. Confirm Linux x86-64, Linux aarch64, Alpine x86-64/aarch64, macOS arm64,
   ASan/UBSan, ThreadSanitizer, signedness, Deno FFI, and inherited Makefile
   qualification.
5. Move the changelog contents from `Unreleased` to a dated `0.3.0` heading,
   tag the qualified commit as `v0.3.0`, and require both tag workflows to pass.
6. Publish only tag-produced archives and the validated benchmark evidence;
   publish the JSR package from the same qualified source revision.
