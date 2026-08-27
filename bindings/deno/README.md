<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Deno binding

> [!WARNING]
> This binding and the normalized native API it calls are
> AGPL-3.0-or-later. The inherited engine, translation bridge, compatibility
> API, and generated index reader remain MPL-2.0. Read the
> [archive notice](NOTICE) before distributing an application or archive.

## Purpose

This module loads the `libmorpheus` shared library with `Deno.dlopen` and
exposes normalized Greek analysis and lemma generation. Native results are
copied into owned TypeScript objects before their C allocations are released.

## Requirements and constraints

- Deno 2 on a 64-bit `x86_64` or `aarch64` runtime.
- A matching ABI 2 `libmorpheus` shared library.
- An Alpheios stemlib tree. Generation additionally requires a validated
  `gener.index` at the stemlib root; analysis remains available without it.
- Greek contexts for generation. Analysis retains the native language support.
- Serialized calls within one context. Distinct contexts may run concurrently.

## Installation

### Standalone release archive

Each GitHub release provides `libmorpheus-deno-<version>.tar.gz` and a companion
SHA-256 file. Extract it beside the matching native archive, import `mod.ts`
from the extracted directory, and pass the installed shared-library path to
`MorpheusLibrary`. The binding archive contains neither native binaries nor
stem data.

### Future JSR package

JSR publication is planned but is not available yet. Until a package name and
release are announced, use the standalone archive or a pinned source checkout;
do not depend on an unversioned repository URL.

## FFI permission

Grant access only to the selected shared library when possible:

```sh
deno run --allow-ffi=/absolute/path/libmorpheus.so app.ts
```

Use the installed platform name, normally `libmorpheus.so` on Linux and
`libmorpheus.dylib` on macOS. A `deno.json` task can retain the same restricted
command for both `x86_64` and `aarch64` deployments; only the library path needs
to change. Environment and file permissions are not required by the binding
itself.

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
  console.log(analyses[0].partOfSpeech);       // "noun"
  console.log(analyses[0].grammaticalNumber); // "singular"
  console.log(analyses[0].grammaticalCases);  // ["nominative"]
} catch (error) {
  if (error instanceof MorpheusError) {
    console.error(`Morpheus status ${error.status}: ${error.message}`);
  } else {
    throw error;
  }
}
```

`analyze()` returns stable English identifiers, arrays for combinable masks,
and `null` for inapplicable scalar values. It preserves all analyses. A generic
stemlib `indecl` class remains `"unknown"` because it does not identify a
lexical category. An empty dialect array means no recorded restriction.

`MorpheusOption.HqDictionary` requires both HQ index files. If they are absent,
the promise rejects with `MorpheusStatus.StemlibError` before native analysis.

## Generate forms from a lemma

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
dual forms are unwanted. The default native limit is 4,096 and the explicit
hard maximum is 65,536.

## Parallel contexts

One context deliberately queues analysis and generation calls because the
native context is stateful. Create separate contexts to perform independent
work in parallel:

```ts
using library = new MorpheusLibrary("/usr/local/lib/libmorpheus.so");
await using first = library.createContext(stemlib, MorpheusLanguage.Greek);
await using second = library.createContext(stemlib, MorpheusLanguage.Greek);

const [analyses, forms] = await Promise.all([
  first.analyze("a)/nqrwpos"),
  second.generate("lo/gos"),
]);
```

The actual speedup depends on the workload and machine. Reuse warm contexts;
the generation index is loaded lazily once per context.

## Raw access and cleanup

Use `analyzeRaw()` and `generateRaw()` for ABI inspection and low-level tools.
They return numeric normalized traits, `structSize`, the complete 11-byte
public morphology vector, and a numeric truncation mask. The semantic methods
return named morphology flags and truncated fields.

Close contexts before their parent library. `using` and `await using` provide
deterministic cleanup, including when a promise rejects. `MorpheusLibrary.close`
rejects while any child context remains open.

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
deno test --allow-env --allow-ffi="$PWD/build/dev/libmorpheus.so" \
  bindings/deno/mod_test.ts
```
