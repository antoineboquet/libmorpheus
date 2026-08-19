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

The `gener` and `gkdict` modules are explicit-return-type boundaries. Their
functions now declare whether they return a status or no value, and `gener`'s
`qsort` call uses a `const void *` comparator adapter matching the
standard-library callback. Both targets treat implicit `int` and incompatible
pointer types as errors.

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

1. Extend the explicit-return-type and callback boundary now used by `gener`
   and `gkdict` to the remaining runtime modules.
2. Correct format strings, buffer bounds, and ownership documentation with
   ASan/UBSan enabled in CI.
3. Move mutable process state into an opaque context, then extract the public
   `libmorpheus` ABI.

Every lot must keep both fixture suites passing: the inherited Perseids
fixtures and the Greek Alpheios stemlib fixtures used by Bailly.
