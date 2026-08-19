# libmorpheus

`libmorpheus` is a modernization of the Morpheus morphological analyzer for
Ancient Greek and Latin. The code baseline comes from the Perseids fork; the
runtime data used by Bailly comes from Alpheios.

The first milestone establishes a reproducible runtime build and behavioral
tests before changing the C dialect or extracting a public FFI API. The current
runtime therefore still uses the inherited GNU C90-compatible mode. The target
is C17 and an embeddable `libmorpheus` with `cruncher` retained as a compatibility
client.

See [the architecture baseline](docs/architecture.md) and
[source provenance](docs/provenance.md) for the boundaries and exact upstream
revisions.

## Clone

The Alpheios stemlib is a pinned Git submodule:

```sh
git clone --recurse-submodules https://github.com/antoineboquet/libmorpheus.git
cd libmorpheus
```

For an existing clone:

```sh
git submodule update --init --recursive
```

## Build the runtime

Requirements:

- CMake 3.25 or newer;
- Ninja;
- a C compiler;
- Ruby 3.0 or newer for tests.

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The resulting executable is `build/dev/cruncher`. The runtime-only CMake build
does not require Flex because it consumes an already compiled stemlib.

To configure against another compiled stemlib:

```sh
cmake --preset dev -DMORPHEUS_STEMLIB_DIR=/absolute/path/to/stemlib
```

`MORPHEUS_STEMLIB_DIR` is used by the Alpheios fixture suite. At runtime,
`cruncher` continues to read the `MORPHLIB` environment variable for backward
compatibility:

```sh
printf 'a)/nqrwpos\n' | \
  MORPHLIB="$PWD/vendor/alpheios-morpheus/dist/stemlib" \
  build/dev/cruncher -S
```

## Tests and data baselines

Two behavioral suites intentionally remain separate:

- `legacy_fixtures` runs the inherited Greek and Latin expectations against
  the Perseids `stemlib` directory;
- `alpheios_greek_fixtures` runs Bailly-oriented Greek smoke cases against
  `vendor/alpheios-morpheus/dist/stemlib`.

This prevents data-version differences from being mistaken for regressions in
the C implementation.

## Historical build

The inherited Makefiles remain available during the transition:

```sh
make -C src clean
CFLAGS='-std=gnu89 -fcommon' make -C src libs
CFLAGS='-std=gnu89 -fcommon' make -C src/anal cruncher
```

## Current command-line interface

```sh
printf 'a)/nqrwpos\n' | \
  MORPHLIB="$PWD/vendor/alpheios-morpheus/dist/stemlib" \
  build/dev/cruncher -S
```

Important options retained from the Perseids implementation include `-L` for
Latin, `-S` for non-strict case, `-n` to ignore accents, `-d` for database
format, `-e` for numeric feature indices, `-k` to retain Beta Code, `-l` for
lemma-only output, and `-V` for verbs only.

The C ABI and structured FFI result model have not yet been introduced; they
are the next milestone after this reproducible baseline.
