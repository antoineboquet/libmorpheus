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

### Alpheios licensing evidence

The Alpheios repository contains conflicting notices that must be preserved as
separate evidence rather than collapsed into a single license conclusion.

- In commit
  [`cfd40b63e4e260824e2b3898403cb83536a0ba5a`](https://github.com/alpheios-project/morpheus/commit/cfd40b63e4e260824e2b3898403cb83536a0ba5a),
  dated 2008-11-24, Bridget Almas added the full MPL-1.1 text and
  [`dist/bin/platform/license.txt`](https://github.com/alpheios-project/morpheus/blob/master/dist/bin/platform/license.txt).
  That notice states that the Morpheus source code was licensed by the Perseus
  Digital Library under MPL-1.1 and identifies Alpheios's XML-output work as
  modifications. Both files remain present on `master`.
- In [issue #72](https://github.com/alpheios-project/morpheus/issues/72), opened
  on 2025-01-24, a contributor reported that the repository lacked a root
  license and that Morpheus ownership and licensing appeared unclear. Maintainer
  `balmas` agreed that the applicable license had never been entirely clear.
- Commit
  [`b78d1ac4fd0a7d145b3b4dde09fa3042b78f9493`](https://github.com/alpheios-project/morpheus/commit/b78d1ac4fd0a7d145b3b4dde09fa3042b78f9493),
  authored by `balmas` on 2025-01-27 to address issue #72, added the root
  [`LICENSE`](https://github.com/alpheios-project/morpheus/blob/master/LICENSE).
  It expressly says that the Alpheios fork does not itself assign a Morpheus
  license, then reproduces the PerseusDL repository's default CC BY-SA 3.0 US
  notice.

The 2025 root file is therefore evidence of a cautious repository-level
interpretation, not a direct relicensing grant from every Morpheus copyright
holder. Issue #72 documents uncertainty rather than resolving the chain of
title. Conversely, the earlier MPL-1.1 notice is specific, contemporaneous with
Alpheios distribution work, and supports the existence of an MPL licensing
tradition, but it does not by itself identify the original grant instrument or
all copyright holders.

These materials strengthen the conservative treatment used here: the
Perseids-derived implementation remains MPL-2.0, and neither the Alpheios root
file nor issue #72 is treated as authority to relicense inherited code under CC
BY-SA. Code and stem data provenance remain separate questions.

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
