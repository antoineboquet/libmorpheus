# Release decision: 0.1.2

Status: candidate prepared for final qualification and publication.

- Project version: **0.1.2**
- C ABI: **1**
- SONAME major: **0**
- 0.1.1 release tag:
  `v0.1.1` at `563bd5c21ee8d3ac7d7d21587254e90f66b4ad04`
- Binding-fix baseline:
  `ace8e715a953a7ae5c407960209b4fd43e1e3ccc`

## Version and ABI rationale

Version 0.1.2 is a compatible Deno-binding patch release. It makes
`hasMorpheusMorphFlag()` accept the analysis arrays returned by both
`analyze()` and `analyzeRaw()`, in addition to an individual analysis.
The native C implementation, public C header, exported-symbol manifest,
numeric constants, and `morpheus_analysis` layout are unchanged from 0.1.1.
ABI version 1 and SONAME 0 therefore remain correct.

## Published scope

The release will publish data-free native archives and SHA-256 integrity files
for Linux x86-64 glibc, Linux aarch64 glibc, and macOS arm64. They must be
rebuilt from the `v0.1.2` tag so their package metadata matches the project
version, even though the native runtime is unchanged.

The tagged source tree provides the corrected Deno binding. Stem data remains
excluded from native archives. The Alpheios stemlib stays pinned at
`4632415fe93c85e9fdca47a0c5a13f31385f0023`.

## Remaining gates

1. Run the complete Linux CI for the release-preparation commit.
2. Manually dispatch the platform workflow with `package_artifacts` enabled
   and inspect the three packages and checksums.
3. Produce and validate the 0.1.2 benchmark report against the accepted 0.1.1
   report on the controlled release host.
4. Tag the qualified commit as `v0.1.2` and require both tag workflows to
   pass.
5. Publish only the tag-produced native archives, their SHA-256 files, and the
   validated benchmark report.