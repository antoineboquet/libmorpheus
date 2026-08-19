# C17 runtime port

## Status

The runtime closure used by `cruncher` builds in C17 mode with `-fno-common`.
This removes the dependency on the GNU `-fcommon` compatibility switch that
the inherited Perseids build required.

This is intentionally not yet a strict-warning-clean C17 port. GCC and Clang
still accept several old declarations as extensions. Treating every warning as
an error now would make the change too large to validate usefully against the
historical analyser.

The `cruncher` front end and the `anal` runtime module are strict boundaries.
Their internal interfaces are declared in `src/anal/cruncher_internal.h` and
`src/anal/anal_internal.h`; both targets compile with
`-Werror=implicit-function-declaration`.

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

1. Extend the explicit internal-interface boundary now used by `cruncher` and
   `anal` to the remaining runtime modules, then enable
   `-Werror=implicit-function-declaration` for the whole runtime.
2. Give every function an explicit return type and correct callbacks passed to
   standard-library functions such as `qsort`.
3. Correct format strings, buffer bounds, and ownership documentation with
   ASan/UBSan enabled in CI.
4. Move mutable process state into an opaque context, then extract the public
   `libmorpheus` ABI.

Every lot must keep both fixture suites passing: the inherited Perseids
fixtures and the Greek Alpheios stemlib fixtures used by Bailly.
