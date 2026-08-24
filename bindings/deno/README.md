# Deno binding

The binding loads the libmorpheus shared library directly with `Deno.dlopen`.
It requires Deno 2 on a 64-bit x86 or ARM runtime.

## License

The original Deno binding code in this directory is licensed under
[AGPL-3.0-or-later](LICENSE). This license applies only to the binding.
The native `libmorpheus` distribution remains under MPL-2.0 and carries its
own license file separately.

```ts
import {
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "./mod.ts";

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
```

Use the platform's installed library name: typically `libmorpheus.so` on
Linux and `libmorpheus.dylib` on macOS. Run applications with FFI permission:

```sh
deno run --allow-ffi app.ts
```

## Standalone release archive

Each GitHub release provides one platform-independent binding archive named
`libmorpheus-deno-<version>.tar.gz`, with a companion SHA-256 file. Extract it
alongside a matching native `libmorpheus` archive, then import `mod.ts` from
the extracted binding directory and pass the installed shared-library path to
`MorpheusLibrary`. The binding archive does not contain a native library or
stem data.

Analysis is declared as a nonblocking FFI call and executes away from Deno's
main event loop. Calls made through one context are serialized by the wrapper
because a native context must not be used concurrently. Separate contexts may
analyze in parallel. Results are copied into TypeScript objects before their
native allocation is released.

`analyze()` returns the ergonomic TypeScript representation: grammatical
values are stable English identifiers, combinable masks are arrays, absent
values are `null` or empty arrays, and morphology flags and truncated fields
are named. `stemType` and `derivationType` remain opaque stemlib identifiers,
represented as `{ code: number }` because ABI version 1 does not assign them a
portable public name.

The semantic part-of-speech names distinguish nouns, verbs, adjectives,
adverbs, articles, pronouns, numerals, prepositions, conjunctions, particles,
and interjections. A stemlib type named only `indecl` remains `"unknown"`
because that morphology class does not identify a lexical category. `degree`
is `null` outside adjectives; irregular comparative and superlative flags are
used when the historical numeric degree field is zero. An empty `dialects`
array means that no specific dialect restriction is recorded. The composite
`"epic"` name consumes its Homeric and non-Homeric epic bits even inside a
larger dialect mask.

Use `analyzeRaw()` when inspecting the ABI or maintaining low-level tooling.
It returns `MorpheusRawAnalysis`, including numeric morphology fields,
`structSize`, the 12- and 14-byte flag vectors, and the numeric truncation mask.

`MorpheusOption.HqDictionary` is conditional on the selected stemlib providing
`hqdict/indices/stindex` and `hqdict/indices/stindex.lindex`. If either file is
missing or empty, `analyze()` and `analyzeRaw()` reject with a `MorpheusError`
whose status is `MorpheusStatus.StemlibError`; the native analyzer is not run.

Close contexts before closing their parent library. `using` and `await using`
provide deterministic cleanup; `MorpheusLibrary.close()` rejects an early
close while contexts remain active.

To check and run the binding tests against a local build:

```sh
deno check bindings/deno/mod.ts bindings/deno/mod_test.ts
MORPHEUS_LIBRARY="$PWD/build/dev/libmorpheus.so" \
MORPHEUS_STEMLIB="$PWD/stemlib" \
deno test --allow-env --allow-ffi bindings/deno/mod_test.ts
```
