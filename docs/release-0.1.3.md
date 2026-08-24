# Release decision: 0.1.3

Status: candidate prepared for final qualification and publication.

- Project version: **0.1.3**
- C ABI: **1**
- SONAME major: **0**
- 0.1.2 release tag:
  `v0.1.2` at `2d59be74ea8fabf091721d5e3f0f2a3d1e99544a`

## Version and ABI rationale

Version 0.1.3 is a compatible maintenance release. It changes the Deno
binding's licensing and source packaging, replaces the Ruby fixture runner with
CMake, and cleans repository metadata and documentation. The native public
header, exported symbols, numeric constants, ownership rules, and the layout of
`morpheus_analysis` are unchanged from 0.1.2. ABI version 1 and SONAME 0
therefore remain correct.

## Candidate scope

- License the original Deno binding under `AGPL-3.0-or-later` while retaining
  MPL-2.0 for the native C implementation.
- Include and verify the binding license and notice in the standalone Deno
  source archive.
- Replace the Ruby behavioral-fixture runner with CMake while preserving the
  inherited and Alpheios fixture suites.
- Verify the Deno archive during ordinary Linux CI as well as release
  qualification.
- Remove obsolete inherited funding, conduct, and Ruby configuration files.
- Describe the project independently of any particular downstream application.

## Distribution contract

The release will publish data-free native archives and SHA-256 integrity files
for Linux x86-64 glibc, Linux aarch64 glibc, and macOS arm64. It will also
publish the platform-independent `libmorpheus-deno-0.1.3.tar.gz` source archive
and its SHA-256 file.

The native archives remain MPL-2.0 licensed. The Deno archive carries its own
AGPL-3.0-or-later license and requires a separately distributed native library
and stemlib. Stem data remains excluded from all release archives. The Alpheios
stemlib stays pinned at
`4632415fe93c85e9fdca47a0c5a13f31385f0023` for qualification.

## Remaining gates

1. Run the complete Linux CI for the release-preparation commit.
2. Manually dispatch the platform workflow with `package_artifacts` enabled
   and inspect the three native archives, the Deno archive, and all checksums.
3. Inspect the extracted Deno archive and confirm that its AGPL license, notice,
   README, and `mod.ts` are complete and mutually consistent.
4. Produce and validate the 0.1.3 benchmark report against the accepted 0.1.2
   report on the controlled release host.
5. Tag the qualified commit as `v0.1.3` and require both tag workflows to pass.
6. Publish only tag-produced archives, their SHA-256 files, and the validated
   benchmark report.
