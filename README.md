# libmorpheus

`libmorpheus` modernizes the Morpheus morphological analyzer for Ancient Greek
and Latin. The code baseline and bundled Greek/Latin stemlib come from the
Perseids-Tools fork; a pinned Greek-only Alpheios stemlib provides an additional
reference dataset for testing and validation.

The runtime now provides:

- a C17 shared library with a versioned, opaque C ABI;
- structured, caller-owned analysis results;
- per-request analysis options;
- indexed Greek lemma generation;
- a Deno 2 FFI binding;
- `cruncher` as a compatibility client of the public library;
- CMake and `pkg-config` installation metadata.

See [the architecture baseline](docs/architecture.md),
[the C17 port notes](docs/c17-port.md), and
[source provenance](docs/provenance.md),
[available stem libraries](docs/stem-libraries.md), and
[platform support](docs/portability.md) for the implementation boundaries,
exact upstream revisions, and tested deployment targets.
The repository is mixed-license: inherited engine, bridge, compatibility, and
derived internal code remain under [MPL-2.0](LICENSE), while the normalized
public API, Deno binding, and marked independently written support files are
licensed under AGPL-3.0-or-later. The exact file boundary and inventory are in
[the licensing guide](docs/licensing.md). No MPL-to-CC change is made here.
Historical evidence supports an MPL licensing lineage for the inherited
Morpheus sources. Later repository-level CC references are recorded as
provenance evidence, but are not treated as authority to relicense those
sources; see [the source-provenance record](docs/provenance.md).

The complete native contract is in [the public API reference](docs/public-api.md);
[release qualification](docs/releasing.md) defines the checks required before
tagging a publishable version, and the [changelog](CHANGELOG.md) records the
candidate release contents.

## Summary

1. [Clone](#clone)
2. [Build and test](#build-and-test)
3. [Install and consume from C](#install-and-consume-from-c)
4. [Deno FFI](#deno-ffi)
5. [`cruncher` compatibility client](#cruncher-compatibility-client)
6. [Alpine container images](#alpine-container-images)
7. [Behavioral baselines](#behavioral-baselines)
8. [Historical build](#historical-build)

## Clone

The Alpheios stemlib is a pinned Git submodule:

```sh
git clone --recurse-submodules https://github.com/defense-humanites/libmorpheus.git
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

The publishable optimized configuration is tested separately from Debug:

```sh
cmake --preset release -DBUILD_TESTING=ON
cmake --build --preset release
ctest --preset release
```

This run also installs into a clean prefix and verifies that no private header
or internal static archive enters the installed package.

To build the versioned native archive and its SHA-256 integrity file after the
Release build:

```sh
cpack --config build/release/CPackConfig.cmake \
  -G TGZ -B build/release/packages
cmake -DMORPHEUS_BUILD_DIR="$PWD/build/release" \
  -DMORPHEUS_PACKAGE_DIR="$PWD/build/release/packages" \
  -P test/test-release-package.cmake
```

Release-tag CI builds and verifies data-free native archives for Linux x86-64
glibc, Linux aarch64 glibc, and macOS arm64, plus a platform-independent Deno
binding source archive. Each archive has a SHA-256 integrity file; the checksum
detects accidental corruption and is not a release signature.

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

Both installed discovery mechanisms are relocatable when the installation
prefix is changed with `cmake --install --prefix`. The test suite compiles,
links, and runs independent consumers through both CMake package discovery and
`pkg-config`, including installations whose library directory has multiple
path components.

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
It loads explicit library and stemlib paths, keeps native resources scoped with
`using`/`await using`, and reports native failures as `MorpheusError` values:

```ts
import {
  MorpheusError,
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "./bindings/deno/mod.ts";

try {
  using library = new MorpheusLibrary("/chosen/prefix/lib/libmorpheus.so");
  await using context = library.createContext(
    "/path/to/stemlib",
    MorpheusLanguage.Greek,
  );

  const analyses = await context.analyze(
    "a)/nqrwpos",
    MorpheusOption.StrictCase,
  );
  console.log(analyses);
} catch (error) {
  if (error instanceof MorpheusError) {
    console.error(`Morpheus status ${error.status}: ${error.message}`);
  } else {
    throw error;
  }
}
```

Run the application with Deno's FFI permission:

```sh
deno run --allow-ffi app.ts
```

The wrapper reads returned native pointers with `Deno.UnsafePointerView`.
Current Deno therefore requires the unscoped FFI permission; a path-scoped
`--allow-ffi=/chosen/prefix/lib/libmorpheus.so` grant lets the library load but
rejects those pointer reads. Keep the library path explicit in application code
and grant no unrelated Deno permissions unless the application needs them.

`analyze()` supports Greek and Latin contexts. The first generation integration
deliberately focuses on Greek and requires a `gener.index` beside its stemlib.
The wrapper dispatches analysis and generation as nonblocking FFI operations,
serializes requests made through one context, and copies native results into
TypeScript objects before releasing native memory. See
[the standalone binding guide](bindings/deno/README.md) for installation,
language scope, all examples, test commands, and platform library names.

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

## Alpine container images

The default multi-stage image builds and tests the C17 runtime on musl, then
ships only the installed library, `cruncher`, its runtime dependencies, and
the pinned Alpheios stemlib:

```sh
docker build --target runtime -t morpheus .
printf 'a)/nqrwpos\n' | docker run --rm -i morpheus -S
```

The Dockerfile is multiarchitecture. With BuildKit/QEMU configured, the same
source builds both immediate Linux targets:

```sh
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  --target runtime \
  .
```

A separate `deno-runtime` target contains Deno, the shared library, the typed
binding, and the stemlib for use as an application base:

```sh
docker build --target deno-runtime -t morpheus-deno .
```

Its `MORPHEUS_LIBRARY` and `MORPHEUS_STEMLIB` variables already point to the
container paths. Application code can import the bundled binding from
`/opt/morpheus/share/morpheus/deno/mod.ts`.

## Behavioral baselines

Two fixture suites, executed by CMake without a scripting-language runtime,
intentionally remain separate:

- `legacy_fixtures` runs the inherited Greek and Latin expectations against
  the Perseids `stemlib` directory;
- `alpheios_greek_fixtures` runs Greek smoke cases against the pinned
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

Only the inherited `libs` and `cruncher` targets are covered by that check.
Other standalone programs preserved below `src/` are deliberately excluded
from the supported build and installation; their status and reintroduction
criteria are documented in [Historical utility policy](docs/historical-utilities.md).
