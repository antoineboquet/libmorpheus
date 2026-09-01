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
separate. The usual explicit setup is:

```sh
npx --package @libmorpheus/node libmorpheus-setup \
  --dataset alpheios --with-gener
export MORPHEUS_LIBRARY="$PWD/morpheus-native/lib/libmorpheus.so"
export MORPHEUS_STEMLIB="$PWD/morpheus-data"
```

Native and data acquisition are also available separately. Acquire the pinned
native runtime with:

```sh
npx --package @libmorpheus/node libmorpheus-native \
  --output ./morpheus-native
export MORPHEUS_LIBRARY="$PWD/morpheus-native/lib/libmorpheus.so"
```

The command refuses an existing destination and verifies the release archive's
SHA-256 sidecar before safely extracting it. Acquire one of the audited,
immutable stem datasets separately:

```sh
npx --package @libmorpheus/node libmorpheus-data \
  --dataset perseids --output ./morpheus-data
export MORPHEUS_STEMLIB="$PWD/morpheus-data"
```

Alpheios provides Greek analysis and supports `--with-gener` for the
experimental generation index; Perseids provides Greek and Latin analysis.
The command verifies the selected file count, every file through a deterministic
tree digest, and the upstream license. Generation additionally verifies its
qualified prepared-source and index digests.

## Build the addon

Configure the repository with a Node installation that provides `node_api.h`:

```sh
cmake -S . -B build/node -G Ninja \
  -DMORPHEUS_BUILD_NODE_BINDING=ON
cmake --build build/node --target libmorpheus_node
```

If CMake cannot infer the header location, set
`MORPHEUS_NODE_INCLUDE_DIR=/path/to/node/include/node`.

## Release model

The binding and its three platform packages share one version but remain
independent of the native runtime version. A `node-v<version>` tag first runs
the general and native-platform qualification workflows, then publishes:

- `@libmorpheus/node-darwin-arm64`;
- `@libmorpheus/node-linux-arm64-gnu`;
- `@libmorpheus/node-linux-x64-gnu`;
- `@libmorpheus/node`, last, after all optional dependencies exist.

Because npm cannot attach a trusted publisher to a package that does not yet
exist, the four names require a one-time interactive bootstrap. Stage inert
`0.0.0` packages, inspect them, and publish them under a non-default tag:

```sh
node bindings/js/node/bootstrap-npm.mjs --output build/npm-bootstrap
for package in build/npm-bootstrap/*; do
  npm publish "$package" --access public --tag bootstrap
done
```

Then configure GitHub trusted publishing on every package with organization
`defense-humanites`, repository `libmorpheus`, workflow filename
`node-release.yml` (filename only), and permission to run `npm publish`.
Subsequent publication uses OIDC and provenance and needs no long-lived npm
token. A manual workflow run with an existing `node-v<version>` tag safely
resumes a partial publication while its qualified artifacts are retained.
