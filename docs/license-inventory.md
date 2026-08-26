<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# License inventory

This inventory records the conservative file-level boundary used by the
current repository. It does not change the license of any expression inherited
from Perseids or any data obtained from Alpheios.

## Independently written files: AGPL-3.0-or-later

The following groups were introduced after the imported Perseids baseline and
have no counterpart at the same path in either audited upstream tree:

- the normalized public ABI in `include/morpheus/morpheus.h` and `src/api/`;
- the Deno binding and its local documentation in `bindings/deno/`;
- the CMake build, package metadata, benchmark tooling, and platform-release
  workflow identified by AGPL SPDX notices;
- the C and CMake tests introduced by the modernization work;
- the deterministic generation-index builder under `tools/`;
- the project changelog and the documents under `docs/`.

`CMakePresets.json` cannot contain comments, so its license is recorded in the
adjacent `CMakePresets.json.license` file.

The classification is based on the preserved Git history: the imported
Perseids baseline is commit `ab6898ffed335fc6169fa02c9940657a9b5a78e0`,
and the independently written support files first appear in later project
commits. Absence from an upstream path is not used on its own to relicense code
that reorganizes or translates historical implementation details.

## Inherited or derived files: MPL-2.0

The following remain MPL-2.0 even where the current path was created locally:

- all inherited implementation files and later modifications to them;
- internal headers that extract declarations or state from the inherited
  engine;
- the legacy-value bridge and compatibility formatter;
- the offline generation-source preparer, whose continuation semantics are
  adapted from the inherited generator;
- inherited workflow, container, README, and repository configuration files;
- `test/fixture.json`, `test/alpheios-fixture.json`,
  `test/gener-fixture.tsv`, the generation-index and generation-source
  fixtures, and the pinned Alpheios data submodule;
- `tools/gener-corpus-manifest.tsv`, which records the ordered paths and
  checksums of derived corpus inputs.

The root `LICENSE` remains the default for every unmarked file. Explicit MPL
notices on boundary files prevent their accidental inclusion in the AGPL set.

## Future PerseusDL-based repository

This inventory makes no MPL-to-CC change. A future repository built from a
separately accepted PerseusDL baseline will require a new inventory and must
not infer its licensing from this one.
