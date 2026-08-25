<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Changelog

This file records user-visible changes to the supported `libmorpheus` runtime.
Historical standalone utilities are outside the release contract. Stemlib data
changes are recorded separately because the native installation does not ship
the corpus.

## [Unreleased]

### Changed

- Native and Deno release archives now carry self-contained licensing and
  provenance information without relying on paths outside the archives.

## [0.2.0] - 2026-08-25

Target project version: **0.2.0**. C ABI: **2**. Shared-library SONAME: **1**.

### Changed

- The structured ABI now uses independently assigned public morphology values;
  historical encodings are translated in an MPL-covered bridge.
- Opaque `stem_type` and `derivation_type` values were removed from
  `morpheus_analysis`.
- Morphology traits now use a complete 84-trait public bitset with zero-based,
  alphabetically ordered indices instead of exposing historical storage.
- Historical formatter declarations moved to `<morpheus/compat.h>` and their
  implementation moved to `src/compat/`.
- The normalized public API, Deno binding, and independently written project
  support files are explicitly AGPL-3.0-or-later; inherited engine, bridge,
  compatibility, fixtures, and derived internal headers remain MPL-2.0.

### Removed

- `morpheus_result_all_morph_flags()`, made redundant by the complete public
  trait bitset embedded in every structured analysis.

### Fixed

- Apple Silicon qualification now validates the current versioned Mach-O
  install name instead of assuming the previous SONAME.

## [0.1.3] - 2026-08-24

Target project version: **0.1.3**. C ABI: **1**. Shared-library SONAME: **0**.

### Added

- The Deno binding is now explicitly licensed under
  `AGPL-3.0-or-later`, independently of the MPL-2.0 native library.
- Linux CI now builds and verifies the standalone Deno binding archive,
  including its license and notice files.

### Changed

- Behavioral fixture tests now use CMake directly and no longer require Ruby.
- Obsolete inherited repository metadata and Ruby configuration were removed.
- Project documentation now describes the library independently of any
  particular downstream application.

### Fixed

- The CMake fixture runner detects optional JSON members without depending on
  version-specific diagnostic text.
- License references in the standalone Deno archive remain valid after
  extraction.

## [0.1.2] - 2026-08-24

Target project version: **0.1.2**. C ABI: **1**. Shared-library SONAME: **0**.

### Added

- A standalone, checksummed Deno binding source archive for each release.

### Fixed

- `hasMorpheusMorphFlag()` now accepts the analysis arrays returned by the Deno
  binding's `analyze()` and `analyzeRaw()` methods, as well as one
  individual analysis.

## [0.1.1] - 2026-08-24

Target project version: **0.1.1**. C ABI: **1**. Shared-library SONAME: **0**.

### Added

- Verified native release packages for Linux x86-64 glibc, Linux aarch64
  glibc, and macOS arm64, each with a SHA-256 integrity file.
- Extracted-package qualification that compiles and runs independent CMake and
  `pkg-config` consumers against every native archive.

### Changed

- Ordinary pull requests use the lower-cost Linux validation tier, while
  manual, weekly, and version-tag runs own the architecture qualification.
- Weekly platform qualification skips the costly matrix when `main` is
  unchanged and the preceding scheduled run succeeded; failed and changed
  revisions are still retried.
- Native package names state the operating system, architecture, and Linux libc
  contract explicitly.
- Linux package checks inspect ELF architecture and runtime dependencies.
  macOS checks inspect the arm64 Mach-O contract, install name, relocatability,
  and macOS 11.0 deployment target.

### Compatibility and data notes

- This release changes packaging and qualification only. The public C ABI
  remains version 1, the shared-library SONAME remains 0, and no runtime source
  or public symbol changed after 0.1.0.
- The Alpheios stemlib remains pinned at
  `4632415fe93c85e9fdca47a0c5a13f31385f0023`; native archives continue to
  exclude stem data.

## [0.1.0] - 2026-08-24

Target project version: **0.1.0**. C ABI: **1**. Shared-library SONAME: **0**.

### Added

- A C17 shared library with a versioned opaque ABI, caller-owned structured
  results, explicit status values, per-request options, and truncation flags.
- A typed Deno 2 FFI binding and a compatibility API used by `cruncher`.
- CMake and relocatable `pkg-config` packages, installed-consumer tests, and
  Alpine multiarchitecture runtime images.
- Native fixture coverage for the inherited Perseids data and the pinned
  Alpheios Greek stemlib.
- Release-comparable JSON benchmarks carrying code, stemlib, compiler, target,
  and runtime metadata.

### Changed

- The Deno `analyze()` result uses named grammatical values, named flag and
  truncation lists, and explicit nullable fields; `analyzeRaw()` preserves the
  numeric ABI representation for low-level consumers.
- Part-of-speech results now distinguish the stemlib's adverbs, articles,
  pronouns, numerals, prepositions, conjunctions, particles, and interjections
  instead of treating all indeclinables as adjectives. The Deno semantic
  result makes degree contextual, honors irregular degree flags, and
  canonicalizes epic submasks in combined dialect values.
- Requests for the optional HQ dictionary now fail once and silently with the
  public stemlib status when its dedicated indices are unavailable, instead of
  repeatedly writing diagnostics and returning an empty successful result.
- `cruncher` is now a client of the public library rather than an independent
  owner of the analyzer runtime.
- Mutable analyzer, dictionary, ending-table, conversion, collation, option,
  statistics, and output state is owned by explicit runtime contexts.
- Active string assembly and result conversion are bounded and transactional;
  capacity failures return errors without publishing partial output.
- The active runtime closure builds as strict ISO C17 with `-fno-common`, strict
  prototypes, conversion diagnostics, and either signed or unsigned `char`.
- Internal static archives remain build-only implementation details. The
  installed surface contains only the shared library, public header, discovery
  metadata, and optional `cruncher` executable.

### Removed from the supported contract

- Historical indexers, generators, converters, GUI programs, and other
  standalone utilities are retained only as classified reference source. They
  are not built, installed, or release-qualified.
- The shared library no longer imports process-terminating `exit()` or
  `abort()` paths; failures propagate through the public status contract.

### Compatibility and data notes

- ABI version 1 is the first published native interface. The pre-1.0 SONAME is
  0; incompatible layout or symbol changes require an explicit ABI review.
- The C implementation derives from Perseids commit
  `ab6898ffed335fc6169fa02c9940657a9b5a78e0`.
- The initially qualified Alpheios stemlib is pinned at
  `4632415fe93c85e9fdca47a0c5a13f31385f0023`. Updating that submodule is a data
  release change and must be reported independently from library code changes.
