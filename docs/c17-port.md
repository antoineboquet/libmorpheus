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
longer consume an indeterminate integer result. Strict missing-return checking
will be enabled after the remaining historical status functions are classified
under full object compilation.

Full object compilation also classifies double augmentation and the legacy Mac
transliteration entry points as in-place procedures. Their contracts now return
`void`; the standalone RTF converter instead returns an explicit success status.

Stem contraction, ending-buffer cleanup, diagnostic output, collation-table
initialization, and stem annotation are likewise side-effect-only procedures;
their definitions and internal prototypes now expose that `void` contract.

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

1. Complete return-value contracts under full object compilation.
2. Correct format strings, buffer bounds, and ownership documentation with
   ASan/UBSan enabled in CI.
3. Move mutable process state into an opaque context, then extract the public
   `libmorpheus` ABI.

Every lot must keep both fixture suites passing: the inherited Perseids
fixtures and the Greek Alpheios stemlib fixtures used by Bailly.
