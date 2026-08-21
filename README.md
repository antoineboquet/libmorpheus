# libmorpheus

`libmorpheus` modernizes the Morpheus morphological analyzer for Ancient
Greek and Latin. The code baseline comes from the Perseids fork; the runtime
data used by Bailly comes from Alpheios.

The runtime now provides:

- a C17 shared library with a versioned, opaque C ABI;
- structured, caller-owned analysis results;
- per-request analysis options;
- a Deno 2 FFI binding;
- `cruncher` as a compatibility client of the public library;
- CMake and `pkg-config` installation metadata.

See [the architecture baseline](docs/architecture.md),
[the C17 port notes](docs/c17-port.md), and
[source provenance](docs/provenance.md) for the implementation boundaries and
exact upstream revisions.

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

## Build and test

Requirements:

- CMake 3.25 or newer;
- Ninja;
- a C17 compiler;
- Ruby 3.0 or newer for the behavioral fixtures;
- Deno 2 only when running the FFI binding tests.

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The runtime-only CMake build consumes an already compiled stemlib and does not
require Flex. It produces `build/dev/libmorpheus.so` (or the platform
equivalent) and, by default, `build/dev/cruncher`.

To configure the Alpheios fixture suite against another compiled stemlib:

```sh
cmake --preset dev -DMORPHEUS_STEMLIB_DIR=/absolute/path/to/stemlib
```

ASan/UBSan and ThreadSanitizer builds have separate presets:

```sh
cmake --preset sanitizers
cmake --build --preset sanitizers
ctest --preset sanitizers

cmake --preset thread-sanitizer
cmake --build --preset thread-sanitizer
ctest --preset thread-sanitizer
```

## Install and consume from C

```sh
cmake --install build/dev --prefix /chosen/prefix
```

The installation contains the shared library, `morpheus/morpheus.h`, the
`Morpheus::morpheus` CMake package target, `libmorpheus.pc`, and
`cruncher`. Stem data remains a separately versioned runtime component and is
not installed with the library.

A CMake consumer can use:

```cmake
find_package(Morpheus 0.1 CONFIG REQUIRED)
target_link_libraries(my_analyzer PRIVATE Morpheus::morpheus)
```

A `pkg-config` consumer can use:

```sh
cc analyzer.c $(pkg-config --cflags --libs libmorpheus)
```

Minimal native use:

```c
#include <stdint.h>
#include <morpheus/morpheus.h>

morpheus_config config = {
  MORPHEUS_ABI_VERSION,
  sizeof config,
  "/path/to/stemlib",
  MORPHEUS_LANGUAGE_GREEK
};
morpheus_context *context = NULL;
morpheus_result *result = NULL;

if (morpheus_open(&config, &context) == MORPHEUS_OK &&
    morpheus_analyze(
      context,
      (const uint8_t *)"a)/nqrwpos",
      sizeof "a)/nqrwpos" - 1,
      MORPHEUS_OPTION_STRICT_CASE,
      &result
    ) == MORPHEUS_OK) {
  for (size_t i = 0; i < morpheus_result_count(result); i++) {
    morpheus_analysis analysis;
    if (morpheus_result_get(result, i, &analysis) == MORPHEUS_OK) {
      /* analysis.lemma, analysis.stem, morphology fields, ... */
    }
  }
}

morpheus_result_free(result);
morpheus_close(context);
```

Results preserve Morpheus ordering and duplicates. The caller owns every
successful result until `morpheus_result_free`. Distinct contexts may be used
concurrently; operations on one context must be serialized.

The installed header documents the complete ABI, error statuses, buffer
contracts, compatibility formatter, and ownership rules.

## Deno FFI

The typed Deno 2 wrapper is in [`bindings/deno/mod.ts`](bindings/deno/mod.ts).
It loads an explicit library path and accepts an explicit stemlib path:

```ts
import {
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "./bindings/deno/mod.ts";

using library = new MorpheusLibrary("/chosen/prefix/lib/libmorpheus.so");
await using context = library.createContext(
  "/path/to/stemlib",
  MorpheusLanguage.Greek,
);

const analyses = await context.analyze(
  "a)/nqrwpos",
  MorpheusOption.StrictCase,
);
```

Run applications with `--allow-ffi`. The wrapper dispatches analysis as a
nonblocking FFI operation, serializes requests made through one context, and
copies native results into TypeScript objects before releasing native memory.
See [the binding guide](bindings/deno/README.md) for test commands and platform
library names.

## cruncher compatibility client

`cruncher` continues to read `MORPHLIB` for command-line compatibility:

```sh
printf 'a)/nqrwpos\n' | \
  MORPHLIB="$PWD/vendor/alpheios-morpheus/dist/stemlib" \
  build/dev/cruncher -S
```

Important retained options include `-L` for Latin, `-S` for non-strict case,
`-n` to ignore accents, `-d` for database format, `-e` for numeric feature
indices, `-k` to retain Beta Code, `-l` for lemma-only output, and `-V` for
verbs only.

## Behavioral baselines

Two fixture suites intentionally remain separate:

- `legacy_fixtures` runs the inherited Greek and Latin expectations against
  the Perseids `stemlib` directory;
- `alpheios_greek_fixtures` runs Bailly-oriented Greek smoke cases against
  `vendor/alpheios-morpheus/dist/stemlib`.

This prevents data-version differences from being mistaken for regressions in
the C implementation. Public ABI, installed-package, parallel-context, Deno,
ASan/UBSan, and TSan tests complement those fixtures.

## Historical build

The inherited Makefiles remain available during the transition:

```sh
make -C src clean
CFLAGS='-std=c17 -fcommon' make -C src libs
CFLAGS='-std=c17 -fcommon' make -C src/anal cruncher
```

They are retained only as a compatibility check. New consumers should use the
CMake build and public ABI.
