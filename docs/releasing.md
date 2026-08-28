<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Release qualification

This checklist defines a publishable `libmorpheus` release. It applies to the
supported C library, installed metadata, Deno binding, containers, `cruncher`,
and the pinned runtime data used by the fixtures. Historical standalone
utilities are outside the release contract.

Each release records its version and ABI decision in
`release-<version>.md`. The current candidate is recorded in
`release-0.3.0.md`; the 0.1.x and 0.2.0 decisions remain historical evidence.

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
- Exercise `tools/prepare-runtime-data.sh` from a recursive checkout and verify
  the generated index digest documented in `runtime-data.md`.
- Confirm that intended release archives or container contexts include the
  pinned Alpheios submodule content; the native install intentionally does not
  install stem data.
- Review `historical-utilities.md` if any standalone entry point was added or
  reactivated.

## 3. Required CI

The Linux workflow in `.github/workflows/test.yml` deliberately has two
levels. Pull requests run the native CMake and Deno jobs. Pushes to `main`
additionally run the optimized build, sanitizers, byte-signedness matrix, and
inherited Makefile compatibility check. Version tags run the same Linux
qualification except that the optimized build and package verification are
owned by the architecture workflow. Superseded runs are cancelled, except for
immutable version-tag runs.

The expensive architecture workflow in `.github/workflows/platform.yml` runs
weekly, on explicit manual dispatch, and for every `v*` tag. It covers native
Linux aarch64, Alpine x86-64/aarch64, and Apple Silicon. Manual dispatches can
request all three native release-candidate packages; version tags always build
them. Packages and checksums are retained as short-lived CI artifacts on non-tag
runs. After a version tag passes the full platform matrix and separate Linux CI,
the tag workflow publishes the verified native and standalone Deno assets to a
GitHub release before it publishes JSR.

Scheduled runs first compare the current default-branch SHA with the last
completed weekly run. The architecture jobs are skipped when the SHA is
unchanged and the previous run succeeded. A changed SHA, a previous failure,
the first scheduled run, a manual dispatch, or a version tag always runs the
matrix. Pull requests changing the architecture workflow execute only this
inexpensive decision job and the Linux x86-64 package qualification. Native
ARM, Alpine, and Apple Silicon remain skipped. The same targeted check runs
when shared package-generation or extracted-consumer scripts change.

Before tagging, manually dispatch the architecture workflow for the exact
candidate commit with `package_artifacts` enabled. Inspect the resulting Linux
x86-64 glibc, Linux aarch64 glibc, and macOS arm64 archives and checksums. The
tagged commit must then rebuild those packages and pass every job triggered by
both workflow files:

- native CMake build and CTest;
- ASan/UBSan and ThreadSanitizer;
- signed- and unsigned-`char` conversion builds;
- Alpine x86-64 and aarch64;
- Apple Silicon arm64;
- Deno type checking and FFI tests;
- inherited Makefile compatibility.

Documentation-only work does not bypass the Linux documentation and release
metadata checks. Do not broaden path exclusions without checking those
contracts first.

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
- Treat container builds as qualification only. Do not publish an image
  embedding the Alpheios stemlib until its redistribution terms have been
  confirmed; this restriction does not apply to data-free native packages.
- Require both images to contain the canonical Alpheios `gener.index` digest.
  Smoke-test `cruncher`, then run real Deno analysis and experimental generation
  against the prepared stemlib, including a dual-form assertion, rather than
  merely checking that Deno starts.
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
  The verification must compile and run independent CMake and `pkg-config`
  consumers against the extracted archive rather than the build-tree install.
  The CI artifact is a release candidate, not a published release or a
  cryptographic signature.

## 6. Publish and verify

- Integrate the release-preparation branch into `main` with a normal merge
  commit. Do not squash or rebase when the branch contains reviewable release
  history that must remain visible.
- Verify after the merge that the former branch tip is an ancestor of `main`
  with `git merge-base --is-ancestor <release-tip> main`.
- Tag the exact commit that passed the complete matrix.
- Build release artifacts from that tag rather than from an uncommitted tree;
  ordinary pull-request and `main` runs do not preserve publishable packages.
- Rebuild all three native archives from the tag; the manually qualified
  artifacts are pre-tag evidence and must not be promoted directly.
- Install the produced native package into a fresh prefix and repeat one CMake
  and one `pkg-config` consumer smoke test.
- Verify the container by digest on both architectures.
- From `bindings/deno`, run `deno publish --dry-run` and inspect the exact file
  list before tagging. Require the configured JSR name and version to match
  `@humanities/libmorpheus` and the CMake project version, respectively.
- Before the first publication, link `@humanities/libmorpheus` to
  `defense-humanites/libmorpheus` in the package's JSR settings. This one-time
  association authorizes tokenless GitHub Actions publication through OIDC.
- Pushing the matching `v<version>` tag automatically publishes the three native
  archives, the standalone Deno archive, and all four SHA-256 sidecars only
  after every platform job and the separate tagged Linux workflow pass. The
  workflow then publishes the JSR package, so its `/native` command never points
  at CI-only artifacts. Its source-only contents retain `README.md`, `LICENSE`,
  and `NOTICE`; they do not embed a native library or stem data. A missing JSR
  repository association makes the JSR publication fail without weakening the
  preceding GitHub release.
- Apply the digest-verification step only if container publication has been
  authorized under the data-distribution policy.
- Preserve the CI run, version/ABI decision, source-data revisions, artifact
  digests, and benchmark comparison with the release notes.
