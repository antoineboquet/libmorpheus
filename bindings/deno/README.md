<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Deno binding

> [!WARNING]
> This binding and the normalized native API it calls are AGPL-3.0-or-later. The
> inherited engine, translation bridge, compatibility API, and generated index
> reader remain MPL-2.0. Read the [archive notice](NOTICE) before distributing
> an application or archive.

## Summary

1. [Purpose](#purpose)
2. [Requirements and constraints](#requirements-and-constraints)
3. [Installation](#installation)
   1. [Standalone release archive](#standalone-release-archive)
   2. [Docker image](#docker-image)
   3. [JSR package](#jsr-package)
4. [Acquire stem data](#acquire-stem-data)
5. [Acquire the native library](#acquire-the-native-library)
6. [Language and stemlib support](#language-and-stemlib-support)
7. [FFI permission](#ffi-permission)
8. [Analyze a form](#analyze-a-form)
9. [Generate forms from a lemma](#generate-forms-from-a-lemma)
10. [Parallel contexts](#parallel-contexts)
11. [Raw access and cleanup](#raw-access-and-cleanup)
12. [Documentation](#documentation)
13. [Local checks](#local-checks)

## Purpose

This module loads the
[`libmorpheus`](https://github.com/defense-humanites/libmorpheus) shared library
with `Deno.dlopen`. It exposes normalized Greek and Latin analysis plus
experimental Greek lemma generation. Native results are copied into owned
TypeScript objects before their C allocations are released.

## Requirements and constraints

- Deno 2 on a 64-bit `x86_64` or `aarch64` runtime.
- A matching ABI 2 `libmorpheus` shared library.
- A compatible stemlib tree. Generation additionally requires a validated
  `gener.index` at the stemlib root; analysis remains available without it.
- Greek contexts for generation. Greek and Latin contexts are supported for
  analysis.
- Serialized calls within one context. Distinct contexts may run concurrently.

## Installation

### Standalone release archive

Each [GitHub release](https://github.com/defense-humanites/libmorpheus/releases)
provides `libmorpheus-deno-<version>.tar.gz` and a companion SHA-256 file.
Extract it beside the matching native archive, import `mod.ts` from the
extracted directory, and pass the installed shared-library path to
`MorpheusLibrary`. The binding archive contains neither native binaries nor stem
data.

### Docker image

The repository's locally built
[`deno-runtime` image target](https://github.com/defense-humanites/libmorpheus/blob/main/Dockerfile)
bundles Deno, the native library, this binding, and the pinned Alpheios stemlib:

```sh
git clone --recurse-submodules \
  https://github.com/defense-humanites/libmorpheus.git
cd libmorpheus
docker build --target deno-runtime -t morpheus-deno .
```

Inside the image, import `/opt/morpheus/share/morpheus/deno/mod.ts`.
`MORPHEUS_LIBRARY` and `MORPHEUS_STEMLIB` already contain the corresponding
container paths. Applications still need Deno's `--allow-ffi` permission. The
image is a qualification and application-build target, not a published registry
image, while the Alpheios redistribution terms remain unresolved.

### JSR package

Install the qualified source binding from
[JSR](https://jsr.io/@humanities/libmorpheus):

```sh
deno add jsr:@humanities/libmorpheus@0.3.0
```

Then import from the dependency name recorded in `deno.json`:

```ts
import {
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "@humanities/libmorpheus";
```

The JSR package contains the TypeScript binding, native and data acquisition
commands, a small internal WebAssembly data preparer, and their licensing and
usage documentation. It does not embed the native library, stem data, or a
generated index. The separately acquired native archive and data directory keep
their own licenses and receipts. A qualified version tag publishes the native
GitHub Release assets first and publishes JSR only after those downloads exist.

## Acquire stem data

The package exports a permission-scoped data command. It downloads an exact
upstream revision, verifies every selected file and the upstream license, and
refuses to overwrite an existing directory. For Greek and Latin analysis:

```sh
deno x \
  --allow-net=codeload.github.com \
  --allow-read=./morpheus-data \
  --allow-write=./morpheus-data \
  jsr:@humanities/libmorpheus@0.3.0/data \
  --dataset perseids \
  --output ./morpheus-data
```

Choose `--dataset alpheios` instead for the Greek-only reference dataset. Pass
the resulting absolute `morpheus-data` path to `createContext()`. The receipt
`MORPHEUS-DATA.json` records the source revision and verified tree digest;
`UPSTREAM-LICENSE` preserves the selected project's license text.

Add `--with-gener` to the Alpheios command to build its experimental Greek
generation index without Git, CMake, or a C compiler:

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

Pass the resulting absolute `morpheus-greek-data` path to `createContext()`. The
command also verifies the pinned Perseids support data in memory, runs the
MPL-covered historical source preparer inside the bundled WebAssembly module,
and builds `gener.index` in TypeScript. It checks the prepared corpus and final
index against the native reference digests. The package does not redistribute
either upstream dataset or the derived index because their distribution terms
require separate review. See the complete
[runtime-data guide](https://github.com/defense-humanites/libmorpheus/blob/main/docs/runtime-data.md)
and
[provenance evidence](https://github.com/defense-humanites/libmorpheus/blob/main/docs/provenance.md).

When working from a full source checkout instead of JSR, the repository also
provides
[`tools/prepare-runtime-data.sh`](https://github.com/defense-humanites/libmorpheus/blob/main/tools/prepare-runtime-data.sh)
for the native build workflow. The JSR data command above is the intended route
for JavaScript users who do not have a C toolchain.

## Acquire the native library

The package can install the matching data-free native release without Git,
CMake, a C compiler, or a shell extractor:

```sh
deno x \
  --allow-net=github.com,release-assets.githubusercontent.com \
  --allow-read=./morpheus-native \
  --allow-write=./morpheus-native \
  jsr:@humanities/libmorpheus@0.3.0/native \
  --output ./morpheus-native
```

The command selects Linux x86-64 glibc, Linux aarch64 glibc, or macOS arm64 from
`Deno.build`, downloads the matching GitHub Release archive and SHA-256 sidecar,
validates the single redirect host, safely extracts the archive, and refuses an
existing output directory. `MORPHEUS-NATIVE.json` records the exact asset,
digest, target, ABI and relative library path.

Use the exported helper to avoid platform-specific filenames:

```ts
import { MorpheusLibrary } from "@humanities/libmorpheus";
import { nativeLibraryPath } from "@humanities/libmorpheus/native";

using library = new MorpheusLibrary(
  nativeLibraryPath("./morpheus-native"),
);
```

Acquisition needs only the scoped network, read and write permissions shown
above. Running an application that loads the result remains a separate step and
requires `--allow-ffi` as described below. The package version fixes the release
version; the command has no arbitrary-version option.

## Language and stemlib support

The operation and selected data source both determine language coverage:

| Operation or dataset                                    | Ancient Greek | Latin | Notes                                                                                  |
| ------------------------------------------------------- | ------------- | ----- | -------------------------------------------------------------------------------------- |
| `analyze()`                                             | Yes           | Yes   | Select `MorpheusLanguage.Greek` or `MorpheusLanguage.Latin` when creating the context. |
| `generate()`                                            | Yes           | No    | The first `gener` integration deliberately focuses on Greek.                           |
| Bundled Perseids-Tools `stemlib/`                       | Yes           | Yes   | Baseline analysis fixtures.                                                            |
| Pinned Alpheios `vendor/alpheios-morpheus/dist/stemlib` | Yes           | No    | Reference fixtures and default Docker dataset.                                         |

The
[stem-library inventory](https://github.com/defense-humanites/libmorpheus/blob/main/docs/stem-libraries.md)
links the original projects and records where each dataset appears in the
repository. A stemlib does not need `gener.index` for analysis; the current
generation service does.

## FFI permission

Run applications with Deno's FFI permission:

```sh
deno run --allow-ffi app.ts
```

Use the installed platform name, normally `libmorpheus.so` on Linux and
`libmorpheus.dylib` on macOS. The binding reads returned pointers with
`Deno.UnsafePointerView`; current Deno releases reject those reads under a
path-scoped grant such as `--allow-ffi=/absolute/path/libmorpheus.so`. Keep the
library path explicit in code, but use the unscoped permission shown above. A
`deno.json` task can retain the same command for both `x86_64` and `aarch64`
deployments; only the library path needs to change. Environment and file
permissions are not required by the binding itself.

## Analyze a form

```ts
import {
  MorpheusError,
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "./mod.ts";

try {
  using library = new MorpheusLibrary("/usr/local/lib/libmorpheus.so");
  await using context = library.createContext(
    "/path/to/stemlib",
    MorpheusLanguage.Greek,
  );

  const analyses = await context.analyze(
    "a)/nqrwpos",
    MorpheusOption.StrictCase,
  );
  console.log(analyses[0].partOfSpeech); // "noun"
  console.log(analyses[0].grammaticalNumber); // "singular"
  console.log(analyses[0].grammaticalCases); // ["nominative"]
} catch (error) {
  if (error instanceof MorpheusError) {
    console.error(`Morpheus status ${error.status}: ${error.message}`);
  } else {
    throw error;
  }
}
```

`analyze()` returns stable English identifiers, arrays for combinable masks, and
`null` for inapplicable scalar values. It preserves all analyses. A generic
stemlib `indecl` class remains `"unknown"` because it does not identify a
lexical category. An empty dialect array means no recorded restriction.

Options are bit flags and may be combined with `|`. For example, strict case
plus accent-insensitive fallback is:

```ts
const options = MorpheusOption.StrictCase |
  MorpheusOption.IgnoreAccents;
const analyses = await context.analyze("a)/nqrwpos", options);
```

Passing no option uses the binding's default analysis behavior. See the
[native option table](https://github.com/defense-humanites/libmorpheus/blob/main/docs/public-api.md#request-options)
before enabling specialized modes.

`MorpheusOption.HqDictionary` requires both HQ index files. If they are absent,
the promise rejects with `MorpheusStatus.StemlibError` before native analysis.

## Generate forms from a lemma

> [!CAUTION]
> `generate()` and `generateRaw()` are experimental. Their automated
> differential, isolation, failure, portability, and sanitizer coverage is
> extensive, but sufficient real-world use is still required before this
> qualification can be removed.

```ts
import {
  MorpheusDialect,
  MorpheusError,
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusNumber,
  MorpheusStatus,
} from "./mod.ts";

try {
  using library = new MorpheusLibrary("/usr/local/lib/libmorpheus.so");
  await using context = library.createContext(
    "/path/to/stemlib",
    MorpheusLanguage.Greek,
  );

  const duals = await context.generate("lo/gos", {
    number: MorpheusNumber.Dual,
    dialect: MorpheusDialect.Attic,
    resultLimit: 256,
  });
  for (const form of duals) console.log(form.surface, form.grammaticalCases);
} catch (error) {
  if (
    error instanceof MorpheusError &&
    error.status === MorpheusStatus.ResultLimitExceeded
  ) {
    console.error("Increase the explicit result limit for this paradigm");
  } else {
    throw error;
  }
}
```

`generate()` is nonblocking and accepts typed filters for part of speech,
dialect, region, person, number, gender, case, tense, mood, voice, and degree.
It preserves dialect masks, duals, duplicate surfaces, and multiple indexed
interpretations unless filters remove them. `excludeDuals` is available when
dual forms are unwanted. The default native limit is 4,096 and the explicit hard
maximum is 65,536.

## Parallel contexts

One context deliberately queues analysis and generation calls because the native
context is stateful. Create separate contexts to perform independent work in
parallel:

```ts
using library = new MorpheusLibrary("/usr/local/lib/libmorpheus.so");
await using first = library.createContext(stemlib, MorpheusLanguage.Greek);
await using second = library.createContext(stemlib, MorpheusLanguage.Greek);

const [analyses, forms] = await Promise.all([
  first.analyze("a)/nqrwpos"),
  second.generate("lo/gos"),
]);
```

The actual speedup depends on the workload and machine. Reuse warm contexts; the
generation index is loaded lazily once per context.

## Raw access and cleanup

Use `analyzeRaw()` and `generateRaw()` for ABI inspection and low-level tools.
They return numeric normalized traits, `structSize`, the complete 11-byte public
morphology vector, and a numeric truncation mask. The semantic methods return
named morphology flags and truncated fields.

Close contexts before their parent library. `using` and `await using` provide
deterministic cleanup, including when a promise rejects. `MorpheusLibrary.close`
rejects while any child context remains open.

## Documentation

| Topic                              | Document                                                                                            |
| ---------------------------------- | --------------------------------------------------------------------------------------------------- |
| Native ABI, ownership, and options | [Public API](https://github.com/defense-humanites/libmorpheus/blob/main/docs/public-api.md)         |
| AGPL/MPL file boundary             | [Licensing guide](https://github.com/defense-humanites/libmorpheus/blob/main/docs/licensing.md)     |
| Source and dataset lineage         | [Provenance](https://github.com/defense-humanites/libmorpheus/blob/main/docs/provenance.md)         |
| Available stem libraries           | [Stem libraries](https://github.com/defense-humanites/libmorpheus/blob/main/docs/stem-libraries.md) |
| Supported platforms                | [Portability](https://github.com/defense-humanites/libmorpheus/blob/main/docs/portability.md)       |
| Release archives and qualification | [Releasing](https://github.com/defense-humanites/libmorpheus/blob/main/docs/releasing.md)           |

## Local checks

Build the library and prepare the small differential generation index before
running the binding tests:

```sh
cmake --preset dev
cmake --build --preset dev
build/dev/morpheus_gener_index_builder \
  stemlib/gener.index test/generation-service-source.txt
deno check bindings/deno/mod.ts bindings/deno/mod_test.ts
MORPHEUS_LIBRARY="$PWD/build/dev/libmorpheus.so" \
MORPHEUS_STEMLIB="$PWD/stemlib" \
deno test --allow-env --allow-ffi \
  bindings/deno/mod_test.ts
```
