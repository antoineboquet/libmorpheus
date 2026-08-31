<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# libmorpheus for Node.js

`@libmorpheus/node` provides a typed ESM facade over the stable libmorpheus C
ABI. Its Node-API addon loads a separately installed native runtime at runtime;
the npm package contains neither `libmorpheus` nor linguistic stem data.

The binding preserves multiple analyses, nullable grammatical traits, dialect
masks, and dual forms. Native analysis and experimental generation execute on
Node's asynchronous worker pool. Calls made through one context are serialized;
distinct contexts may execute concurrently.

## Development use

The npm package selects a matching optional Node-API binary package on Linux
x64 glibc, Linux arm64 glibc, or macOS arm64. Installation runs no scripts. A
source build can instead select its addon explicitly with
`MORPHEUS_NODE_ADDON`:

```sh
export MORPHEUS_NODE_ADDON=/path/to/libmorpheus_node.node
```

Then provide the independently installed native library and stemlib:

```js
import {
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "@libmorpheus/node";

const library = new MorpheusLibrary(process.env.MORPHEUS_LIBRARY);
const context = library.createContext(
  process.env.MORPHEUS_STEMLIB,
  MorpheusLanguage.Greek,
);

try {
  const analyses = await context.analyze(
    "a)/nqrwpos",
    MorpheusOption.StrictCase,
  );
} finally {
  await context.close();
  library.close();
}
```

Package installation runs no native installation script. The optional package
contains only the small Node-API adapter; `libmorpheus` and stem data remain
separate. The runtime/data setup command will be added before the first npm
publication.

## Build the addon

Configure the repository with a Node installation that provides `node_api.h`:

```sh
cmake -S . -B build/node -G Ninja \
  -DMORPHEUS_BUILD_NODE_BINDING=ON
cmake --build build/node --target libmorpheus_node
```

If CMake cannot infer the header location, set
`MORPHEUS_NODE_INCLUDE_DIR=/path/to/node/include/node`.
