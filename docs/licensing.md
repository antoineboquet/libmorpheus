# Licensing boundary

This repository is a mixed-license work. A file's SPDX identifier controls
when present; files without one remain under the repository's default
MPL-2.0 license in `LICENSE`.

## AGPL-3.0-or-later

The independently written normalized API and Deno integration are licensed
under AGPL-3.0-or-later:

- `include/morpheus/morpheus.h`;
- `src/api/`;
- `bindings/deno/`.

These files expose context and result ownership, status handling, normalized
analysis records, and language bindings without publishing internal stemlib
identifiers or storage encodings.

## MPL-2.0

The inherited analyzer and all code that preserves or translates its
historical representations remain under MPL-2.0, including:

- the inherited implementation outside the explicitly AGPL directories;
- `src/bridge/legacy_values.c` and `src/bridge/legacy_values.h`;
- `include/morpheus/compat.h` and `src/compat/compat.c`;
- the historical `cruncher` behavior.

The bridge is intentionally the only structured-API component that knows both
the public values and the historical numeric encodings. This placement does
not relicense inherited expressions; it isolates them in MPL-covered files.

## License texts and provenance

Canonical texts are available in `LICENSES/MPL-2.0.txt` and
`LICENSES/AGPL-3.0-or-later.txt`. The root `LICENSE` remains MPL-2.0 so that
unmarked inherited files keep their existing treatment. Source ancestry is
recorded in `docs/provenance.md`.

No file in this repository is relicensed from MPL-2.0 to CC BY-SA. Any future
repository based on a separately accepted PerseusDL baseline requires its own
provenance and licensing decision.
