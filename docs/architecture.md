# Architecture baseline

This document records the architecture inherited from the Perseids version of
Morpheus before functional modernization begins.

## Runtime boundary

The runtime currently consists of the `cruncher` command-line front end and six
groups of C sources:

- `anal`: morphological analysis and legacy result formatting;
- `gener`: generation of candidate forms;
- `gkends`: ending lookup and matching;
- `gkdict`: stem and derivation lookup;
- `morphlib`: shared morphology, data access, and encoding utilities;
- `greeklib`: Beta Code and Greek-language utilities.

The CMake build deliberately compiles only this runtime closure. The historical
Make build also builds the tools used to compile stem data, including Flex
lexers. Those tools are not required to run `cruncher` against an already
compiled stemlib.

## Data source

The runtime data used by Bailly is the precompiled Greek stemlib from
`alpheios-project/morpheus`, under `dist/stemlib/Greek`. The Alpheios repository
is pinned as the `vendor/alpheios-morpheus` Git submodule. The inherited
top-level `stemlib` directory comes from Perseids and is not the default runtime
data source.

## Modernization boundary

The intended architecture extracts the analyzer behind a stable C ABI named
`libmorpheus`. `cruncher` will become a compatibility client of that ABI, while
the Bailly Deno API will consume structured results through FFI.

The initial CMake milestone established a reproducible runtime build and a
behavioral baseline. The runtime now builds in C17 mode and rejects historical
common-symbol linkage (`-fno-common`), while the inherited K&R-style
definitions and compiler extensions are removed incrementally. The extraction
of the public API remains a separately testable change.

## Known constraints

The inherited engine contains process-wide mutable state, lazy global caches,
and static formatting buffers. It is not currently reentrant or thread-safe.
The first FFI implementation must serialize calls. True per-context concurrency
requires moving this state into an opaque analyzer context.
