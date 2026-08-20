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

The first context boundary now owns the active language selection, the lazy
Beta Code collation tables, the dynamically allocated morphology-flag lookup
tables, the language-specific raw-preverb table, and both directions of the
SmartA/SMK conversion state. It also owns the language-specific morphology-key
tables and their sorted lookup index, plus file-open diagnostics and legacy
volume-name state. The language-specific vowel-contraction and consonant-
euphony tables used while composing endings are context-owned as well. The
loaded ending-table cache and its circular replacement state are also isolated
per context. The verb, nominal, and lemma dictionary pre-index tables are
context-owned and language-aware as well. The legacy `set_lang` and `cur_lang`
entry points dispatch
through an opaque runtime context, and context activation is thread-local.
Context destruction releases allocated tables, including the
inverse converter's 512 lookup strings, while a language change reloads the
preverb, morphology-key, contraction, euphony, ending-table, and dictionary
pre-index data on their next use.
The derivation subsystem's circular result cache, diagnostic counters, and
reduplication scratch buffers are context-owned too; only the result cache is
language-invalidated because the scratch buffers are cleared before each use.
The lazily loaded compound-noun head table is dynamically sized and owned by
the active context; teardown releases both its entries and pointer index.
Buffered analysis output and its previous-analysis formatting markers are now
context-owned as well, preventing one active context from exposing another's
most recently rendered result.
The reusable augmented-stem and possible-stem work arrays used by verb analysis
also belong to the active context and are released during context teardown.
The three reusable irregular-form and irregular-key buffers follow the same
ownership and teardown rules.
The crasis-analysis disable option is stored in the active context, so changing
it no longer affects other contexts in the process.
Ending-selection work records are call-local, while the prefix-matching mode
shared by `setwendstr` and `endstrcmp` belongs to the active context.
The suffix-table input stream and its unavailable marker are also context-owned
and the stream is closed when its context is destroyed.
The reusable ending-construction store, its entry count, and maximum serialized
length belong to the active context and are released during teardown.
The six lazily loaded ending and stem indexes are context-owned and their text
buffers, pointer tables, and descriptors are released during teardown.
Accepted-analysis and distinct-lemma totals, along with the sticky allocation
error state, belong to the active runtime context.
Greek-string clearing now uses a call-local zero value, and sorted insertion
invokes its caller-supplied comparator directly instead of storing it globally.
Recursive ending generation now uses call-local unrestricted-form workspace,
eliminating its four remaining shared Greek-string records.
The requested-dialect mask used by analysis belongs to the active runtime
context and defaults independently to all dialects.
Dictionary-entry generation uses call-local word and blank-ending records, so
concurrent callers no longer overwrite a shared temporary word.
Dormant memory counters, debug switches, and blank analysis records left over
from the legacy front ends have been removed from the runtime objects.
Dictionary HQ selection is now an active-context option and no longer changes
dictionary lookup mode for unrelated analyzers.
Binary lookup no longer exposes a process-wide diagnostic switch; lookup
behavior is covered directly without mutable debug state.
The crasis and enclitic-suffix rule tables are private immutable data rather
than exported writable arrays.
The analyzer's quick-search mode is selected on the active context, preserving
both the exhaustive and early-return paths without process-wide state.
The runtime libraries no longer retain writable file-scope analysis state,
apart from the explicitly thread-local default and active context selectors.
The compatibility CLI keeps its timing workspace local to `main`, and the
compile-disabled dictionary stem-cache implementation has been removed.
