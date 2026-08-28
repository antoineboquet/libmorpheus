<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Acquiring runtime data

The native archives and the JSR package intentionally contain no linguistic
data. Applications select a compiled stem library at runtime; experimental
Greek generation additionally requires a validated `gener.index` at that
stemlib's root.

## Choose a dataset

| Use | Dataset path in a recursive `libmorpheus` checkout | Languages | `gener.index` |
| --- | --- | --- | --- |
| Analysis | `stemlib/` | Ancient Greek and Latin | Not required |
| Reference analysis | `vendor/alpheios-morpheus/dist/stemlib` | Ancient Greek | Not required |
| Experimental generation | Prepared directory described below | Ancient Greek | Included after local preparation |

Clone the release that matches the native library or JSR binding:

```sh
git clone --depth 1 --branch v0.3.0 --recurse-submodules \
  --shallow-submodules \
  https://github.com/defense-humanites/libmorpheus.git libmorpheus-data
```

For Greek and Latin analysis, pass the absolute path to
`libmorpheus-data/stemlib` when creating a context. For the pinned Alpheios
Greek reference data, use
`libmorpheus-data/vendor/alpheios-morpheus/dist/stemlib` instead.

## Prepare Greek generation data

From the recursive release checkout, run:

```sh
sh libmorpheus-data/tools/prepare-runtime-data.sh "$PWD/morpheus-greek-data"
```

The command requires CMake, Ninja, and a C compiler. It builds only the narrow
internal preparation tools, verifies every pinned Alpheios source-file digest,
builds the complete generation corpus twice, checks deterministic byte
identity and corpus counts, and copies the Alpheios Greek compiled stemlib plus
the validated index into the new output directory. It refuses an existing
output directory rather than overwriting data.

Use the resulting absolute `morpheus-greek-data` path for both analysis and
experimental generation. The expected SHA-256 of its `gener.index` in version
0.3.0 is:

```text
5aa76d8c86c54af5121a3cce506ecaa57d14c6667ac0f091efd164ddfa9822d6
```

The preparer and index builder remain internal build-time programs: they are
not installed and are not part of the C or Deno public API.

## Distribution boundary

The prepared directory contains Alpheios linguistic data and an index derived
from it. `libmorpheus` does not place either one in native archives, the JSR
package, or downloadable release assets. Review the [provenance and licensing
evidence](provenance.md) before redistributing a prepared directory. The local
preparation workflow does not change the MPL/AGPL code boundary described in
the [licensing guide](licensing.md).

See the [stem-library inventory](stem-libraries.md) for upstream repositories
and the language coverage of the three principal datasets.
