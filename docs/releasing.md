# Release qualification

This checklist defines a publishable `libmorpheus` release. It applies to the
supported C library, installed metadata, Deno binding, containers, `cruncher`,
and the pinned runtime data used by the fixtures. Historical standalone
utilities are outside the release contract.

## 1. Version and ABI decision

- Choose the project version and update `project(VERSION ...)` in
  `CMakeLists.txt`.
- Review the diff of `include/morpheus/morpheus.h` and
  `cmake/PublicApi.cmake`.
- Keep `MORPHEUS_ABI_VERSION` and `SOVERSION` unchanged only for compatible
  additions. A removed function, changed signature, reassigned constant, or
  changed `morpheus_analysis` layout requires an explicit ABI/SONAME decision.
- Confirm that the Deno `ABI_VERSION` and native structure declarations match
  the C header.
- Write release notes that distinguish code changes from stemlib data changes.
- Move the candidate notes in `CHANGELOG.md` from `Unreleased` to a dated
  version heading without changing their technical scope during packaging.

## 2. Source and data provenance

- Start from a clean checkout with recursive submodules initialized.
- Record the Perseids code baseline and Alpheios stemlib commit documented in
  `provenance.md`.
- Confirm that intended release archives or container contexts include the
  pinned Alpheios submodule content; the native install intentionally does not
  install stem data.
- Review `historical-utilities.md` if any standalone entry point was added or
  reactivated.

## 3. Required CI

The release commit must pass every job in `.github/workflows/test.yml`:

- native CMake build and CTest;
- ASan/UBSan and ThreadSanitizer;
- signed- and unsigned-`char` conversion builds;
- Alpine x86-64 and aarch64;
- Apple Silicon arm64;
- Deno type checking and FFI tests;
- inherited Makefile compatibility.

Do not qualify from a partial rerun that omits a failing job. Both Perseids and
Alpheios fixture suites must run where their data prerequisites are available.

## 4. ABI and consumer checks

- `public_symbols` must contain exactly the functions in
  `cmake/PublicApi.cmake` and no imported `exit` or `abort`.
- `public_api_documentation` must confirm that the header, symbol manifest, and
  `public-api.md` cover the same functions.
- The installed CMake consumer must configure, build, link, and run through
  `Morpheus::morpheus`.
- `installed_surface` must confirm that no private header, internal archive,
  static library, or source-tree path enters the installation.
- The installed `pkg-config` consumer must compile, link, and run, and the
  nested-libdir test must resolve the relocated prefix correctly.
- Confirm that the generated project version, package-config version, SONAME,
  and `libmorpheus.pc` version agree.
- Require the `release_metadata` test to pass; it keeps the C header, Deno
  binding, public API documentation, changelog, and CMake ABI decisions aligned.

## 5. Runtime artifacts

- Build the runtime and `deno-runtime` container targets for linux/amd64 and
  linux/arm64.
- Smoke-test `cruncher` and the Deno wrapper against the stemlib intended for
  Bailly, not merely the small inherited fixture tree.
- Inspect the native installation and confirm it contains only the public
  header, shared library, CMake package files, `libmorpheus.pc`, and optional
  `cruncher` executable.
- Require the optimized `Release` CTest job to pass; Debug and sanitizer
  success does not substitute for testing the configuration that is published.
- Produce a versioned JSON benchmark report as described in `benchmarks.md`,
  including source revision, stemlib revision, and compiler metadata. Compare
  it with the previous accepted report on the same controlled hardware and
  investigate material regressions before tagging.
- Pass the final JSON report through `bench/validate.ts` with the exact source,
  stemlib, compiler, and label values before preserving it as release evidence.
- Generate the native archive with `cpack` and require
  `test-release-package.cmake` to verify its checksum and installed surface.
  The CI artifact is a release candidate, not a published release or a
  cryptographic signature.

## 6. Publish and verify

- Integrate `agent/c17-runtime-port` into `main` with a merge commit preserving
  its complete ancestry. Do not squash or rebase the modernization commits.
  The intended operation is equivalent to
  `git merge --no-ff agent/c17-runtime-port`.
- Verify after the merge that the former branch tip is an ancestor of `main`
  with `git merge-base --is-ancestor <runtime-tip> main`.
- Tag the exact commit that passed the complete matrix.
- Build release artifacts from that tag rather than from an uncommitted tree.
- Rebuild the native archive from the tag; do not promote an artifact produced
  by an earlier pull-request run.
- Install the produced native package into a fresh prefix and repeat one CMake
  and one `pkg-config` consumer smoke test.
- Verify the container by digest on both architectures.
- Preserve the CI run, version/ABI decision, source-data revisions, artifact
  digests, and benchmark comparison with the release notes.
