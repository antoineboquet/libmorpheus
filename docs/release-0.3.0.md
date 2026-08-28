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

The Deno source package is configured for JSR under the reserved name
`@humanities/libmorpheus` at candidate version 0.3.0. Its metadata, exact
source-only contents, import command, and strict registry dry-run are covered
by the ordinary Linux CI. The dry-run contains only `jsr.json`, `mod.ts`,
`README.md`, `LICENSE`, and `NOTICE`. The JSR package retains the AGPL notice
and the documented MPL boundary to the separately distributed native runtime.

Container builds remain qualification artifacts. An image embedding the pinned
Alpheios stemlib must not be published until that dataset's redistribution terms
have been confirmed.

## Remaining gates

1. Produce and validate the 0.3.0 benchmark report on the controlled release
   host against the accepted 0.2.0 schema 1 report (SHA-256
   `d877dadd080a31ae75c8f970fb179a6638b54a2db0afd11c336eb9b2e33cf977`).
   Compare its historical analysis measurements; treat the schema 2 generation
   measurements as the initial accepted generation baseline.
2. Manually dispatch the architecture workflow for the exact candidate commit
   with package artifacts enabled and inspect every archive and checksum.
3. Confirm Linux x86-64, Linux aarch64, Alpine x86-64/aarch64, macOS arm64,
   ASan/UBSan, ThreadSanitizer, signedness, Deno FFI, and inherited Makefile
   qualification.
4. Move the changelog contents from `Unreleased` to a dated `0.3.0` heading,
   tag the qualified commit as `v0.3.0`, and require both tag workflows to pass.
5. Publish only tag-produced archives and the validated benchmark evidence;
   let the tagged platform workflow publish the JSR package from the same
   qualified source revision after tagged Linux CI succeeds. The JSR package
   must first be linked to this GitHub repository in its JSR settings.
