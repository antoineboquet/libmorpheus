# Release decision: 0.2.0

Status: unreleased candidate.

- Project version: **0.2.0**
- C ABI: **2**
- SONAME major: **1**

## Version and ABI rationale

The normalized analysis record and several published constants intentionally
change. Opaque stemlib identifiers and the raw complete-bitset accessor are
removed. A new ABI number and SONAME are therefore required. Because the
project remains pre-1.0, the feature release advances to 0.2.0.

Consumers must rebuild against `<morpheus/morpheus.h>` ABI 2. Historical
formatter consumers must additionally include `<morpheus/compat.h>`.

## Licensing boundary

The independently written normalized API and Deno binding are marked
AGPL-3.0-or-later. The inherited engine, historical compatibility layer, and
numeric translation bridge remain MPL-2.0. The root MPL license remains the
default for unmarked inherited files. This release makes no MPL-to-CC change.

## Qualification required before tagging

- Complete the ordinary, sanitizer, and thread-sanitizer CMake presets.
- Run the independent installed CMake and `pkg-config` consumers.
- Run Deno type checking and FFI tests against ABI 2.
- Verify exported symbols and native-package license contents.
- Re-run differential fixtures and the platform release matrix.

This document records the current source-tree decision; it is not a statement
that the 0.2.0 artifacts have already been published or qualified.
