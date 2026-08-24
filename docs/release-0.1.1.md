# Release decision: 0.1.1

Status: candidate prepared for history-preserving integration, final platform
qualification, and benchmark review.

- Project version: **0.1.1**
- C ABI: **1**
- SONAME major: **0**
- 0.1.0 tag:
  `54f936a2b674c61d0949e51455ca0f4f2aa8b99c`
- Post-0.1.0 qualification baseline:
  `191172633f9c867a25702c026dac57de44fcccd9`

## Version and ABI rationale

Version 0.1.1 is a compatible packaging and release-qualification update. The
public header, exported-symbol manifest, numeric constants, and
`morpheus_analysis` layout have not changed since 0.1.0. ABI version 1 and
SONAME 0 therefore remain correct. Native consumers may upgrade without
recompiling for an ABI transition.

The CMake project version, generated CMake and `pkg-config` metadata, package
filenames, changelog, public API documentation, and this decision are checked
together by the `release_metadata` test.

## Published scope

The release publishes three data-free native archives and their SHA-256
integrity files:

- `libmorpheus-0.1.1-Linux-x86_64-glibc.tar.gz`;
- `libmorpheus-0.1.1-Linux-aarch64-glibc.tar.gz`;
- `libmorpheus-0.1.1-macOS-arm64.tar.gz`.

Each archive contains the shared library, public C header, CMake and
`pkg-config` discovery metadata, and `cruncher`. The Deno binding remains
available from the tagged source tree. Stem data is not included.

The Alpheios stemlib remains pinned at
`4632415fe93c85e9fdca47a0c5a13f31385f0023` for fixtures and Bailly
compatibility. Container images embedding that data remain qualification-only
until redistribution is authorized.

## Remaining gates

1. Merge the release-preparation pull request with a normal merge commit,
   without squash or rebase.
2. Run the final benchmark from a clean checkout of that exact merge commit and
   compare it with the accepted 0.1.0 baseline.
3. Manually dispatch the complete platform workflow for that exact commit with
   `package_artifacts` enabled and inspect all three archives and checksums.
4. Tag the qualified commit as `v0.1.1`; require the immutable tag workflows to
   rebuild and verify every published archive.
5. Publish only the tag-produced archives and their checksums.
