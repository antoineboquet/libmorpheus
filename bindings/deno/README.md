# Deno binding

The binding loads the installed shared library directly with `Deno.dlopen`.
It requires Deno 2 and a 64-bit x86 or ARM runtime.

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

Run applications with the FFI permission, for example:

```sh
deno run --allow-ffi app.ts
```

Analysis executes on a Deno blocking thread. Calls made through one context are
serialized by the wrapper because a context must not be used concurrently.
Separate contexts may analyze in parallel. Results are copied into TypeScript
objects before their native allocation is released. Close contexts before
closing their parent library; `using` and `await using` provide deterministic
cleanup.
