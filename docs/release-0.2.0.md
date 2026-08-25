<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Release decision: 0.2.0

Status: candidate prepared for final qualification and publication.

- Project version: **0.2.0**
- C ABI: **2**
- SONAME major: **1**
- 0.1.3 release tag:
  `v0.1.3` at `5b88cbec171f4ccade0b08edccbaf1763e1eb90b`

## Version and ABI rationale

The normalized analysis record and several published constants intentionally
change. Opaque stemlib identifiers and the raw complete-bitset accessor are
removed. A new ABI number and SONAME are therefore required. Because the
project remains pre-1.0, the feature release advances to 0.2.0.

Consumers must rebuild against `<morpheus/morpheus.h>` ABI 2. Historical
formatter consumers must additionally include `<morpheus/compat.h>`.

## Candidate scope

- Publish independently assigned public morphology values instead of the
  historical engine encodings.
- Remove opaque stem and derivation identifiers from the structured analysis
  record.
- Replace the sparse historical morphology flags with a complete, zero-based
  public trait bitset.
- Move the historical formatter to the explicit compatibility API in
  `<morpheus/compat.h>`.
- Separate the normalized ABI from the inherited representation through an
  MPL-covered numeric translation bridge.
- Apply AGPL-3.0-or-later to the independently written API, Deno binding, and
  marked project-support files while preserving MPL-2.0 for inherited and
  derived expressions.

## Licensing boundary

The independently written normalized API, Deno binding, tests, build and
release infrastructure, benchmark tooling, and project documentation are
marked AGPL-3.0-or-later. The inherited engine, historical compatibility layer,
numeric translation bridge, derived internal headers, and fixtures remain
MPL-2.0. The root MPL license remains the default for unmarked inherited files.
This release makes no MPL-to-CC change. The complete rationale is recorded in
`license-inventory.md`.

## Distribution contract

The release will publish data-free native archives and SHA-256 integrity files
for Linux x86-64 glibc, Linux aarch64 glibc, and macOS arm64. It will also
publish the platform-independent `libmorpheus-deno-0.2.0.tar.gz` source archive
and its SHA-256 file.

The native archives contain the normalized public header, the explicit
compatibility header, the shared library, CMake and `pkg-config` metadata,
`cruncher`, and the MPL and AGPL licensing materials. Stem data remains
excluded. The Alpheios stemlib stays pinned at
`4632415fe93c85e9fdca47a0c5a13f31385f0023` for qualification.

## Benchmark evidence

The preparation benchmark produced from
`e991c0e15fa164f58f7a56a61a19ccfe106dfe73` on Apple Silicon reports stable
performance relative to the accepted baseline: FFI throughput changes by
approximately +1.9%, +0.7%, and 0.0% for one, two, and four contexts. Persistent
`cruncher` throughput improves by approximately 1.6%, and cold invocation by
approximately 4.0%. Peak RSS changes remain near 1%.

Because this decision and changelog finalization create a descendant commit,
the final archived benchmark must be regenerated from the exact commit selected
for the `v0.2.0` tag. The preparation report remains valid performance evidence
but must not be labelled as the tag-exact report.

## Remaining gates

1. Run the complete Linux CI for the release-preparation commit.
2. Manually dispatch the platform workflow with `package_artifacts` enabled
   and inspect the three native archives, the Deno archive, and all checksums.
3. Regenerate and validate the 0.2.0 benchmark report from that exact commit on
   the controlled release host.
4. Tag the qualified commit as `v0.2.0` and require both tag workflows to pass.
5. Publish only tag-produced archives, their SHA-256 files, and the validated
   tag-exact benchmark report.

This document records the current source-tree decision; it is not a statement
that the 0.2.0 artifacts have already been published or qualified.
