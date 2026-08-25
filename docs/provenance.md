<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Source provenance

The repository preserves the Git history of the Perseids Morpheus fork and uses
the Alpheios precompiled stemlib as a separately versioned data dependency.

## C implementation

- Repository: <https://github.com/perseids-tools/morpheus.git>
- Branch: `master`
- Imported commit: `ab6898ffed335fc6169fa02c9940657a9b5a78e0`
- License declared by that repository: Mozilla Public License 2.0

## Greek stemlib

- Repository: <https://github.com/alpheios-project/morpheus.git>
- Branch tracked by the submodule: `master`
- Initially pinned commit: `4632415fe93c85e9fdca47a0c5a13f31385f0023`
- Runtime path: `vendor/alpheios-morpheus/dist/stemlib/Greek`

The submodule commit, rather than the moving branch name, defines the exact data
version used by any given `libmorpheus` revision. Updating to a newer Alpheios
`master` must be committed as an explicit submodule update and followed by the
behavioral test suite.

The Alpheios repository's `LICENSE` file notes that its Morpheus fork does not
carry an explicit independent license and refers to the Perseus Digital Library
licensing statement. Code and data provenance must therefore remain documented
separately when this project is distributed.

## Local licensing boundary

The current repository continues to treat the Perseids-derived implementation
as MPL-2.0. Independently written normalized API files are identified with
AGPL-3.0-or-later SPDX headers. Historical numeric translations and formatter
compatibility remain in explicitly MPL-covered bridge and compatibility files;
see `docs/licensing.md`.

The same conservative review was applied to support files introduced after the
imported baseline. Independently written build, test, benchmark, release, and
documentation files carry AGPL notices. New internal headers that extract or
reorganize inherited declarations remain explicitly MPL, regardless of their
later creation date. The auditable classification and its exclusions are in
`docs/license-inventory.md`.

No claim about a CC-licensed PerseusDL baseline is applied to this repository.
