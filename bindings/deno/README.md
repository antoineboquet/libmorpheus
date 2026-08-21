# Deno binding

The binding loads the libmorpheus shared library directly with `Deno.dlopen`.
It requires Deno 2 on a 64-bit x86 or ARM runtime.

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
```

Use the platform's installed library name: typically `libmorpheus.so` on
Linux and `libmorpheus.dylib` on macOS. Run applications with FFI permission:

```sh
deno run --allow-ffi app.ts
```

Analysis is declared as a nonblocking FFI call and executes away from Deno's
main event loop. Calls made through one context are serialized by the wrapper
because a native context must not be used concurrently. Separate contexts may
analyze in parallel. Results are copied into TypeScript objects before their
native allocation is released.

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
