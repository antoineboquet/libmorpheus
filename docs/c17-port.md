# C17 runtime port

## Status

The runtime closure used by `cruncher` builds in C17 mode with `-fno-common`.
This removes the dependency on the GNU `-fcommon` compatibility switch that
the inherited Perseids build required.

This is intentionally not yet a strict-warning-clean C17 port. GCC and Clang
still accept several old declarations as extensions. Treating every warning as
an error now would make the change too large to validate usefully against the
historical analyser.

The entire `cruncher` runtime closure is now a strict boundary. Its internal
interfaces are declared in `src/anal/cruncher_internal.h`,
`src/anal/anal_internal.h`,
`src/gkdict/gkdict_internal.h`, `src/gkends/gkends_internal.h`, and
`src/gener/gener_internal.h`, with the Greek primitives collected by
`src/greeklib/greeklib_internal.h` and the morphology interfaces by
`src/morphlib/morphlib_internal.h`. All seven targets compile with
`-Werror=implicit-function-declaration`.

`src/includes/greek.h` now has an include guard, allowing these interfaces to
share its historical type definitions safely. The `addaccent` and `cinsert`
prototypes were also aligned with their existing `int` parameters.

Declaring the `morphlib` boundary exposed two more incomplete historical calls.
`fixacc.c` passed a third argument to the two-argument `getsyll` and `getsyll2`
implementations; the unused extra arguments were removed. `PrntDialect` called
the three-argument `AddDialect` without its delimiter; it now passes the space
separator expected by the formatted output. The `AddDialect` prototype was
aligned with that implementation.

All six runtime modules are explicit-return-type boundaries. Their definitions
no longer rely on implicit `int`, and their `qsort` calls use `const void *`
comparators matching the standard-library callback. All six targets treat
implicit `int` and incompatible pointer types as errors.

Typing `anal` also made its historical common-prefix structure conversions
visible. Calls that deliberately pass a generated word or analysis to helpers
which inspect only the embedded `gk_string` prefix now use explicit casts;
the corresponding allocations and layouts are unchanged.

Typing `greeklib` distinguishes in-place string mutators from queries and I/O
status functions. Pure mutators now return `void`; capitalization normalizers
return an explicit success flag, preserving their historical zero result when
no conversion is possible.

Typing `morphlib` also exposed a disk-format width mismatch: `ReadKey` stores a
32-bit offset, while `endtags` keeps the value in a `long`. `retrentry.c` now
reads through an `int` temporary before assigning the structure field, instead
of passing a `long *` to the 32-bit reader.

The first return-contract pass identified allocation, accentuation,
transliteration, and preverb helpers that only mutate their arguments. These
procedures and their public prototypes now return `void`, so callers can no
longer consume an indeterminate integer result.

Full object compilation also classifies double augmentation and the legacy Mac
transliteration entry points as in-place procedures. Their contracts now return
`void`; the standalone RTF converter instead returns an explicit success status.

Stem contraction, ending-buffer cleanup, diagnostic output, collation-table
initialization, and stem annotation are likewise side-effect-only procedures;
their definitions and internal prototypes now expose that `void` contract.

The same classification covers movable-nu insertion, present reduplication,
language selection, flag rendering, phonetic normalization, and whitespace
cleanup. Character-level SMK conversion remains a query and now returns its
input explicitly when no special mapping applies.

Index construction and file-count queries now return explicit success or count
values, while record emission, system-path rewriting, and comparison-table
initialization are declared as `void` procedures.

Morphology-key initialization and domain-name rendering now expose their
side-effect-only contracts. The first analysis pass and its unused phrase hook
also return `void`, while the public analysis entry points retain their counts.

Morphology-flag mutation, table initialization, debugging, and name rendering
now return `void`. The `has_morphflags` predicate instead returns an explicit
false value when no requested bit is present.

The final `gkstring` pass classifies deallocation, shallow analysis copying,
formatted output, and buffer-appending helpers as procedures. `low_bit_of`
remains an integer query and now returns zero when no bit is selected. With all
118 runtime sources passing full object compilation, all six libraries enforce
`-Werror=return-type` in CMake.

The first format-contract pass aligns `size_t` diagnostics with `%zu`, makes
32-bit and native offsets explicit at variadic call sites, and supplies the
missing word argument in an ending-retrieval diagnostic. The legacy numeric
ending index now copies the `word_form` bitfield representation into a checked
unsigned scalar before printing it, instead of passing a structure through a
variadic integer conversion. Local font and domain-table constants were also
renamed to avoid collisions with public macros.

Bounded filename construction now rejects oversized converter and ending-table
paths before opening files. Ending-table display lines and irregular-generation
diagnostics use buffers large enough for the declared maximum component sizes.
The same ending-table error path no longer attempts to close a null stream when
an input file cannot be opened. This removes 14 of the 16 library
format-overflow diagnostics.

The remaining compound-lemma and derivation-key builders now use scratch
buffers matching their lookup contracts, then reject values that cannot fit in
the historical destination fields. They no longer truncate keys or pass a
60-byte lemma buffer to a lookup routine that may emit `LONGSTRING` bytes. The
command-line client applies the same checked construction to input, output,
failure, statistics, and destination paths; its destination mode now initializes
all three output names. All 118 library sources plus `stdiomorph.c` pass with
format, overflow, and truncation warnings promoted to errors. CMake enforces
these checks across the runtime.

## Buffer and allocation ownership

The runtime keeps its historical caller-supplied buffer model. Functions that
receive a writable `char *` do not allocate or retain that buffer; the caller
owns its storage and must provide the capacity stated by the interface, normally
`MAXWORDSIZE` or `LONGSTRING`. Checked path and compound-key builders now fail
instead of truncating when that capacity is insufficient.

`CreatGkString`, `CreatGkAnal`, and `CreatGkword` return heap allocations.
`FreeGkString`, `FreeGkAnal`, and `FreeGkword` release them; `FreeGkword` also
owns and releases its attached analysis array and odd-key buffer. `CpGkAnal` is
a shallow copy of the analysis pointer and count, so it does not create a second
independently owned analysis array.

The `sanitizers` CMake preset instruments the full runtime with ASan and UBSan.
CI runs both fixture suites with immediate failure on memory or undefined-
behavior reports. The isolated runtime-context lifecycle test also enables leak
detection; the full analyser retains it disabled until its process-lifetime
caches have moved behind the same teardown boundary.

The first instrumented fixture pass found and fixed three lifetime violations:
empty ending strings no longer read before their stack buffer, preverb suffix
checks now verify the available length before computing a suffix pointer, and
the accent-insensitive comparison buffer remains alive until its final use.
The opaque runtime context owns language selection, the lazily initialized Beta
Code collation tables, the dynamically allocated morphology-flag lookup tables,
the language-specific raw-preverb table, and both directions of SmartA/SMK
conversion state. The inverse converter's 512 allocated lookup strings are
released with their owning context. The language-specific morphology-key
tables and their sorted pointer index are context-owned as well. Contexts have
explicit create, activate, and destroy operations; file-open diagnostics and
legacy volume-name state are context-owned as well. The ending composer's
language-specific vowel-contraction and consonant-euphony tables now share the
same ownership and teardown boundary. The 45-entry ending-table cache and its
circular replacement state are context-owned too. The three dictionary
pre-indexes for verbs, nominals, and lemmas now belong to the active context and
reload when its language changes. Derivation-result caching, counters, and the
eight reusable reduplication buffers are isolated and released with the context
as well. The compound-noun head table is also context-owned, dynamically grown,
and released at teardown. Buffered analysis output and its formatting cursors
are isolated and released with their active context. The reusable augmented-
stem and possible-stem analysis buffers share the same ownership boundary.
The reusable irregular-verb form and key buffers are context-owned as well.
The crasis-analysis disable option is isolated per runtime context.
Ending-selection scratch records are call-local, and its prefix-match state is
isolated per runtime context.
Suffix-table iteration keeps its stream and unavailable state in the active
context, with teardown closing an open stream.
The ending-construction store and its counters are isolated and released with
their owning runtime context.
The six ending and stem indexes loaded by `endindex` follow the same ownership
and teardown boundary.
Analysis and lemma accounting, including the sticky storage-error marker, is
isolated per runtime context.
Greek-string reset and sorted insertion no longer rely on shared workspace or a
process-global comparator callback.
Recursive ending generation also uses call-local unrestricted-form workspace
instead of static Greek-string records.
The analysis dialect-selection mask is isolated per runtime context while
preserving the historical all-dialects default.
Dictionary-entry generation no longer stores its temporary word and blank
ending in process-wide objects.
Unused analyzer memory counters, debug state, and blank work records were
removed instead of carrying inert shared state into the library ABI.
The dictionary HQ mode and its cache invalidation snapshot now live in the
active runtime context.
The unused `BinLookPrnt` diagnostic global and its conditional output paths
were removed, with exact and lower-bound lookup behavior covered by a test.
The six built-in crasis and enclitic-suffix tables are now `static const`, with
const-correct helper boundaries for their string fields.
Activation is thread-local,
while the historical `set_lang` and `cur_lang` calls remain compatible.
Destruction releases the allocated tables, and switching a context's language
invalidates its preverb, morphology-key, contraction, euphony, ending-table,
and dictionary-index data on the next lookup. Other mutable caches still need
to migrate before the analyser is reentrant.

Typing `gkends` exposed a `gk_string *` passed to `FixRecAcc`, which requires a
`gk_word *` and accesses fields beyond the smaller structure. `contract.c` now
constructs the required temporary word and copies the ending metadata before
accentuation.

Declaring the `gkends` boundary exposed one incomplete historical call:
`mkend.c` invoked `do_dissim` without the function's required `Stemtype`.
The call now passes `stemtype_of(Have)`, which is the ending metadata inspected
by `do_dissim` when handling aorist passive participles.

## Scope of the runtime closure

The CMake runtime consists only of the sources linked by `cruncher`:

- `anal`, `gener`, `gkends`, `gkdict`, `morphlib`, and `greeklib`;
- the `stdiomorph.c` command-line client.

The stemlib compiler, Flex lexers, platform-specific front ends, and unused
analysis utilities are outside this porting scope. They may be migrated later
without blocking the library intended for Bailly.

## Verified linkage constraint

`-fno-common` links the `cruncher` runtime. Two duplicate tentative globals
(`Gstr` and `endlines`) remain in index-building utilities
(`countendtables.c` and `indexendtables.c`). They are not linked into the
runtime closure and must be resolved when the stemlib build tools are ported.

## Next compatibility-preserving lots

1. Move the remaining mutable caches and formatting state into the opaque
   context, then extract the public `libmorpheus` ABI.

Every lot must keep both fixture suites passing: the inherited Perseids
fixtures and the Greek Alpheios stemlib fixtures used by Bailly.
