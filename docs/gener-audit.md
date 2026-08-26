<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# `gener` integration audit

Date: 2026-08-25

## Scope and baselines

This audit covers the Greek generation path in `src/gener`, its stemlib inputs,
and the work needed to expose lemma-to-form generation through the normalized
C API and Deno binding. Latin shares much of the engine but is not the first
integration target. The supplied stemlib has no Italian data.

The source comparison used these repository heads:

- PerseusDL: `b1b33c56ef2338fe0dcd1893628ed638f00c0986`;
- Perseids Tools: `ab6898ffed335fc6169fa02c9940657a9b5a78e0`;
- Alpheios: `2f1a30d65ed7ae9c6120dbf64d730b863be412e4`;
- the current `libmorpheus` tree after the 0.2.0 release.

The inherited generator and adaptations of it remain MPL-2.0. A new normalized
API and binding may use the existing AGPL/MPL boundary: public types and API
orchestration can be independently written under AGPL-3.0-or-later, while the
generator, source-format translation, and historical compatibility behavior
remain under MPL-2.0.

## Main conclusion

Integration is feasible, but the historical executable is not a
`lemma -> forms` query program. It is the final stage of a stem-source build
pipeline. It consumes blocks containing a lemma plus explicit stem records and
expands those records through the ending tables.

The reusable core is already partly present in the supported build:
`GenStemForms()` and `GenIrregForm()` live in `genwd.c`, which is the only
`src/gener` translation unit currently compiled by CMake. A production API
must add a reverse lookup from a lemma to its generation records. The installed
runtime stem indices are organized for form analysis, not for that reverse
lookup, and the Alpheios stemlib used here does not contain the historical
`hqdict/hqdict` word list expected by `getlemmstart()`.

The recommended design is therefore to build a compact, sorted generation
index from the stem sources during stemlib preparation. Runtime generation can
then look up one lemma, parse its already-expanded records, and call the typed
generation core. Porting the stateful `do_conj` and `gener` text front ends
into every runtime process is neither necessary nor desirable.

## What the historical programs actually do

Generation is a two-stage pipeline:

1. `do_conj` expands derived verb entries (`:de:` plus `;` and `@` continuation
   records) into explicit verb stems (`:vs:`), using `derivs/out`.
2. `gener` parses source blocks and calls `GenStemForms()` for regular noun,
   adjective, and verb stems, or `GenIrregForm()` for `:wd:` and `:vb:` records.

The relevant source tags are:

| Tag | Meaning in the generator |
| --- | --- |
| `:le:` | Begin a lemma block. |
| `:no:` | Regular nominal stem. |
| `:aj:` | Regular adjectival stem. |
| `:vs:` | Explicit regular verb stem. |
| `:wd:` | Stored indeclinable form. |
| `:vb:` | Stored irregular verb form. |
| `:de:` | Derived verb specification consumed by `do_conj`, not directly expanded by `gener`. |
| `;` | Principal-part request inherited by `do_conj`. |
| `@` | Continuation or override of the preceding source record. |

`GenStemForms()` does not search for a lemma. Its caller must supply a
populated `gk_word` and the historical ASCII keys describing one stem. It
retrieves compatible endings, combines dialect constraints, creates augmented
or preverb variants, applies accentuation, and returns a sentinel-terminated
array of `gk_word` values. `GenIrregForm()` parses and returns a stored form,
with special handling for indeclinables.

The `mode` argument to `GenStemForms()` is currently unused. For irregular
forms, only the `INDECL` bit has an effect.

## Observed behavior

A separately built Perseids reference executable was run against the pinned
Alpheios stemlib.

- `lo/gos` with `:no:log os_ou masc` produced 15 rows, including Attic,
  Doric, Aeolic, Epic, and Ionic variants.
- Two explicit irregular records for `ei)mi/` produced two corresponding
  rows; irregular generation does not infer the rest of the paradigm.
- A controlled `lu/w` entry expanded by `do_conj` for present, future, and
  aorist stems produced 826 morphological rows, 702 distinct surface strings,
  and about 50 KiB of text.

These results establish several API requirements:

- one surface string may have several valid morphological records;
- dialect variants must not be deduplicated by spelling alone;
- results can be large even for one lemma;
- ordering and resource limits must be specified;
- the caller needs filters rather than a single unqualified "all forms"
  operation.

The historical `FULL_DUMP` formatter explicitly omits every dual form even
though the generation core creates duals. A structured API should include
duals by default and provide an explicit filter if a consumer wants a reduced
paradigm. It must not inherit this formatter loss silently.

The Greek stem sources used by their historical build rules contain 65,743
lemma blocks for 64,376 exact lemma keys. There are 1,289 keys occurring in
more than one block, and one block (`i(/hmi`) contains 544 explicit form or
stem records. A reverse index must therefore support multiple blocks per key
and must not assume a small fixed result count.

## Provenance comparison

The generator is old inherited code. The history of `genermain.c` reaches the
initial 1997 repository import. The operative generation algorithms are shared
across the PerseusDL, Perseids, and Alpheios lines.

Compared with PerseusDL, Perseids mainly replaced unbounded `strcpy` calls with
the project `Xstr*` wrappers. Alpheios later added three narrowly relevant
changes:

- replace two interactive `gets()` calls with bounded `fgets()` handling;
- add `<stdlib.h>` to `genermain.c`;
- give the `qsort` comparator in `genwd.c` a compatible `const void *`
  signature.

Alpheios also committed generated objects and executables; these binaries are
not source evidence and should not be imported. The current tree already has a
typed comparator wrapper and stronger checks in `genwd.c`, so the Alpheios
comparator change is functionally covered. Its `gets()` fixes concern legacy
helper executables that are not in the supported CMake build.

The current `genwd.c` additionally contains the modernization work needed by
the analyzer: explicit prototypes, bounded workspaces, allocation checks,
runtime error propagation, and context-scoped caches. Those modifications are
adaptations of the inherited generator and remain on the MPL side of the
boundary.

## Build and code maturity

There are 17 historical `.c` files under `src/gener`. With the project's strict
C17 diagnostics, only `genwd.c` currently passes a standalone syntax check.
The other files retain combinations of K&R definitions, implicit declarations,
missing return types, unsafe formatting, or obsolete input routines.

CMake intentionally builds only:

```text
morpheus_gener = src/gener/genwd.c
```

The historical Makefile additionally builds `gener`, `do_conj`, `checkstype`,
and several unsupported helpers. Those front ends have process-level behavior
that is unsuitable for an in-process API:

- `genermain.c` calls `exit()` and derives an output pathname in an 80-byte
  buffer;
- `gensynform.c` stores parser and formatter state in file-scope statics;
- `conjsys.c` stores the current lemma, stems, keys, and principal-part state
  in file-scope buffers;
- `genmisc.c` can create diagnostic files such as `bumwords` and `oddkeys2`;
- several helpers print diagnostics directly to stdout or stderr.

These files should remain provenance references unless a fixture tool needs
them. They should not be the implementation of the public API.

## Data contract and reverse lookup

The existing runtime files `steminds/nomind` and `steminds/vbind` map possible
stems to lemmas for analysis. They do not provide a lossless lemma-to-source
record mapping. `getlemmstart()` implements the reverse direction only through
the separate `hqdict/hqdict` word list, which is absent from the pinned
Alpheios stemlib.

Reading the many raw `stemsrc` files at runtime would introduce source-file
selection rules, duplicate merging, verb derivation expansion, and large
startup or query costs into the library. It would also make behavior depend on
build inputs that are not currently part of the validated runtime contract.

The stemlib preparation step should instead emit a versioned generation index
with these properties:

- sorted normalized lemma keys with an offset table;
- every source block retained for duplicate lemma keys;
- `:de:` records expanded to the explicit `:vs:` records needed at runtime;
- regular and irregular record types preserved;
- source order or an explicit stable order recorded;
- a format version, language marker, and integrity checks;
- deterministic generation from the pinned stemlib commit.

The generated index is data derived from the stemlib inputs. Its licensing and
redistribution terms must follow those inputs; it is not part of the new AGPL
API merely because the index builder is newly written.

## Reentrancy and ownership risks

`genwd.c` now uses the thread-local active runtime context for ending tables,
language selection, error state, and caches. This is compatible with the public
rule that distinct contexts may run concurrently while one context must be
serialized.

The generation functions still mutate the supplied `gk_word` while exploring
forms and return internal `gk_word` arrays. Copies of `gk_word` are shallow for
the `oddkeys` and `analysis` pointers. The public layer must therefore:

- create all input words within the call;
- convert every returned form to normalized owned records before releasing
  internal storage;
- never expose `gk_word`, historical numeric values, or borrowed pointers;
- restore context options on every exit path;
- add failure-injection and sanitizer coverage for every allocation boundary.

The stateful `gensynform.c` and `conjsys.c` front ends are not reentrant and
must stay outside the request path.

## Recommended public contract

The first supported feature should be Greek lemma generation in Beta Code,
matching the current native text contract. Unicode conversion belongs in the
downstream application unless a separate encoding API is designed.

An additive 0.3.0 API can keep ABI version 2 and SONAME 1 if it only adds new
symbols and opaque result types. A later implementation review must confirm
this before release. The API should provide:

- an explicit-length lemma input with embedded-NUL rejection;
- a generation-options structure carrying its size and version;
- dialect, part-of-speech, tense, mood, voice, person, number, gender, case,
  and degree filters;
- an explicit choice concerning dual forms, defaulting to inclusion;
- a documented maximum result count and a distinct limit-exceeded status, or
  pagination/callback iteration that preserves deterministic order;
- one normalized record per morphological interpretation, even when surface
  strings repeat;
- stable ordering with a textual tie-breaker rather than relying on `qsort`
  equivalence;
- an owned opaque result independent of its context after success.

Generated records can reuse the normalized grammatical values and public trait
translation introduced for analyses. They should use a generation-specific
public name or opaque accessor surface so that analysis-only semantics do not
become accidental generation promises.

## Implementation sequence

1. **Freeze differential fixtures.** Record exact outputs for regular nouns,
   adjectives, explicit verb stems, derived verbs, preverbs, augments,
   indeclinables, irregular verbs, duplicate lemmas, dialects, duals, and very
   large paradigms. Compare Perseids and current core behavior.
2. **Specify and build the reverse index.** Implement a deterministic offline
   stemlib tool, validate its format, and pin fixture checksums. Keep verb
   derivation expansion offline.
3. **Add an internal generation service.** Parse indexed records into
   call-local `gk_word` values and invoke only `GenStemForms()` and
   `GenIrregForm()`. Add context isolation, failure, and sanitizer tests.
4. **Normalize results at the bridge.** Reuse the ABI 2 value translators,
   remove stem and derivation codes, define stable ordering, and enforce result
   limits.
5. **Publish the additive C API.** Add installed-consumer, symbol-surface,
   ownership, invalid-input, and package tests.
6. **Extend the Deno binding.** Keep generation nonblocking, expose typed
   filters, and test concurrent use through distinct contexts.
7. **Qualify 0.3.0.** Benchmark representative small and maximal paradigms,
   memory use, cold index loading, warm lookups, and concurrent contexts on the
   qualified platforms.

## Initial fixture baseline

The first executable baseline covers a regular noun, a regular adjective, an
explicit verb stem, an indeclinable, and an irregular verb record. Each case
pins the total row count, distinct surface count, non-dual equivalents from the
Perseids `FULL_DUMP` output, and a deterministic fingerprint of the complete
internal result sequence against the pinned Alpheios stemlib. The regular cases
confirm that the core produces the dual rows suppressed by the historical
formatter.

This baseline deliberately exercises source records directly. Derived verbs,
preverbs, augments, duplicate lemma blocks, and maximal paradigms will be added
when the offline expansion and reverse-index formats can be tested without
making the unsupported historical front ends part of the runtime build.

The version 1 reverse-index format and its offline builder are now specified in
[Generation index format](gener-index-format.md). The initial builder preserves
duplicate lemma blocks, canonicalizes keys like the historical formatter,
emits host-independent little-endian tables, and rejects unexpanded derivation
records. Connecting the complete ordered corpus and a supported derivation
expander remains the next data-preparation step.

The complete pinned source universe is now recorded in
[Greek generation corpus](gener-corpus.md) with per-file SHA-256 digests and a
CI-enforced historical order. It contains both continuation expansion and
derivation expansion work; the nominal/verb manifest groups cannot be treated
as strict part-of-speech partitions.

The bounded offline source preparer now expands direct `@` continuations with
the exact historical base-record concatenation rule. It resets state at lemma
and file boundaries and rejects the corpus's one orphaned continuation rather
than reproducing `gensynform.c`'s stale global-buffer behavior. Continuations
inside `:de:`/`;` sequences remain reserved for the derivation-expansion step.

## Decision

Proceed with integration, beginning with fixtures and the reverse generation
index. Do not port the historical `gener` and `do_conj` command-line programs
wholesale, do not expose their text format as the primary API, and do not make
raw stem sources a runtime dependency.
