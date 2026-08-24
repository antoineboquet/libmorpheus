# Changelog

This file records user-visible changes to the supported `libmorpheus` runtime.
Historical standalone utilities are outside the release contract. Stemlib data
changes are recorded separately because the native installation does not ship
the corpus.

## [Unreleased]

Target project version: **0.1.0**. C ABI: **1**. Shared-library SONAME: **0**.

### Added

- A C17 shared library with a versioned opaque ABI, caller-owned structured
  results, explicit status values, per-request options, and truncation flags.
- A typed Deno 2 FFI binding and a compatibility API used by `cruncher`.
- CMake and relocatable `pkg-config` packages, installed-consumer tests, and
  Alpine multiarchitecture runtime images.
- Native fixture coverage for the inherited Perseids data and the pinned
  Alpheios Greek stemlib used by Bailly.
- Release-comparable JSON benchmarks carrying code, stemlib, compiler, target,
  and runtime metadata.

### Changed

- The Deno `analyze()` result uses named grammatical values, named flag and
  truncation lists, and explicit nullable fields; `analyzeRaw()` preserves the
  numeric ABI representation for low-level consumers.
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
