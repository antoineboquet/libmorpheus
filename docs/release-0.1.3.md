# Release decision: 0.1.3

Status: development candidate; qualification and publication are pending.

- Project version: **0.1.3**
- C ABI: **1**
- SONAME major: **0**
- 0.1.2 release tag:
  `v0.1.2` at `2d59be74ea8fabf091721d5e3f0f2a3d1e99544a`

## Version and ABI rationale

Version 0.1.3 is a compatible maintenance release. It changes repository
tooling, Deno binding licensing, and source packaging without changing the
native public header, exported symbols, numeric constants, ownership rules, or
the layout of `morpheus_analysis`. The C ABI therefore remains 1 and the
shared-library SONAME remains 0.

## Candidate scope

- License the original Deno binding code under `AGPL-3.0-or-later` while
  retaining MPL-2.0 for the native library.
- Include and verify the AGPL license in the standalone Deno source archive.
- Replace the Ruby behavioral-fixture runner with CMake while preserving both
  the legacy and Alpheios fixture suites.
- Remove obsolete inherited funding, conduct, and Ruby configuration files.
- Verify the Deno archive as part of ordinary Linux CI.

## Distribution contract

The native release archives remain data-free and MPL-2.0 licensed. The
platform-independent Deno binding archive is source-only, carries its own AGPL
license, and requires a separately distributed native library and stemlib.
Each archive must retain its companion SHA-256 integrity file.

## Qualification still required

Before tagging `v0.1.3`:

- complete Linux CI on the final candidate revision;
- run the full platform and release qualification workflow;
- review the source and license contents of the extracted Deno archive;
- record a benchmark whose source revision is the final candidate revision;
- update this decision to identify the final candidate and qualification
  results.
