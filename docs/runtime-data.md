<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Acquiring runtime data

The native archives and the JSR package intentionally contain no linguistic
data. Applications select a compiled stem library at runtime; experimental Greek
generation additionally requires a validated `gener.index` at that stemlib's
root.

## Choose a dataset

| Use                     | Dataset path in a recursive `libmorpheus` checkout | Languages               | `gener.index`                    |
| ----------------------- | -------------------------------------------------- | ----------------------- | -------------------------------- |
| Analysis                | `stemlib/`                                         | Ancient Greek and Latin | Not required                     |
| Reference analysis      | `vendor/alpheios-morpheus/dist/stemlib`            | Ancient Greek           | Not required                     |
| Experimental generation | Prepared directory described below                 | Ancient Greek           | Included after local preparation |

JSR users can acquire either dataset without Git or a C toolchain. For Greek and
Latin analysis:

```sh
deno x \
  --allow-net=codeload.github.com \
  --allow-read=./morpheus-data \
  --allow-write=./morpheus-data \
  jsr:@humanities/libmorpheus@0.3.0/data \
  --dataset perseids \
  --output ./morpheus-data
```

Use `--dataset alpheios` for the Greek-only reference dataset. The command
downloads a pinned archive directly from the original GitHub repository, checks
the complete selected tree and license, and refuses an existing output
directory. Pass the resulting absolute `morpheus-data` path to the context.

A recursive checkout of the matching release remains useful for native
development and for the current generation-index preparation workflow:

```sh
git clone --depth 1 --branch v0.3.0 --recurse-submodules \
  --shallow-submodules \
  https://github.com/defense-humanites/libmorpheus.git libmorpheus-data
```

## Prepare Greek generation data

JSR users can add `--with-gener` to the Alpheios acquisition command:

```sh
deno x \
  --allow-net=codeload.github.com \
  --allow-read=./morpheus-greek-data \
  --allow-write=./morpheus-greek-data \
  jsr:@humanities/libmorpheus@0.3.0/data \
  --dataset alpheios \
  --with-gener \
  --output ./morpheus-greek-data
```

This route needs no C toolchain. It downloads the pinned Alpheios data directly
for the output and the pinned Perseids data as in-memory support for historical
derivation rules. The bundled internal WebAssembly preparer expands the source
corpus; an internal TypeScript builder emits the index. Both intermediate and
final digests must match the native reference before the command succeeds. It
refuses an existing output directory rather than overwriting data.

Native developers can produce the same bytes from a recursive release checkout
with CMake, Ninja, and a C compiler:

```sh
sh libmorpheus-data/tools/prepare-runtime-data.sh "$PWD/morpheus-greek-data"
```

Use the resulting absolute `morpheus-greek-data` path for both analysis and
experimental generation. The expected SHA-256 of its `gener.index` in version
0.3.0 is:

```text
5aa76d8c86c54af5121a3cce506ecaa57d14c6667ac0f091efd164ddfa9822d6
```

The native preparer and index builder remain internal build-time programs. The
JSR command exposes only the acquisition operation; its WebAssembly preparer and
TypeScript index builder remain internal modules, not part of the C or default
Deno API.

## Distribution boundary

The prepared directory contains Alpheios linguistic data and an index derived
from it. `libmorpheus` does not place either one in native archives, the JSR
package, or downloadable release assets. Review the
[provenance and licensing evidence](provenance.md) before redistributing a
prepared directory. The local preparation workflow does not change the MPL/AGPL
code boundary described in the [licensing guide](licensing.md).

See the [stem-library inventory](stem-libraries.md) for upstream repositories
and the language coverage of the three principal datasets.
