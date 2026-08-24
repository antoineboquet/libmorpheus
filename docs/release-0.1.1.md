# Release decision: 0.1.1

Status: published.

- Project version: **0.1.1**
- C ABI: **1**
- SONAME major: **0**
- Release tag: `v0.1.1`
- Release commit:
  `563bd5c21ee8d3ac7d7d21587254e90f66b4ad04`
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

The release also archives the validated 0.1.1 benchmark report together with
the release assets. The benchmark protocol records the exact source and stemlib
revisions, compiler, platform, corpus identity, and run parameters; its
comparison against the accepted 0.1.0 baseline is part of the release record.

The Alpheios stemlib remains pinned at
`4632415fe93c85e9fdca47a0c5a13f31385f0023` for fixtures and Bailly
compatibility. Container images embedding that data remain qualification-only
until redistribution is authorized.

## Completion record

- The release-preparation history was integrated into `main` with the normal
  merge commit `563bd5c21ee8d3ac7d7d21587254e90f66b4ad04`.
- The final benchmark was produced from the qualified release source and
  archived with the release evidence.
- Linux x86-64 glibc, Linux aarch64 glibc, and macOS arm64 packages were
  qualified, including extracted CMake and `pkg-config` consumer checks and
  platform binary-contract checks.
- The immutable `v0.1.1` tag identifies the qualified source revision.
- The GitHub release publishes only the tag-produced native archives, their
  SHA-256 integrity files, and the relevant benchmark reports.
