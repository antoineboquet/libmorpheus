# Public analysis values

The shared library uses hidden visibility by default and exports only the
functions declared with `MORPHEUS_API` in `<morpheus/morpheus.h>`. Its current
project version is 0.1.0 and its pre-1.0 ABI SONAME is 0. Adding an exported
function is compatible within that SONAME; changing or removing one, or
changing the layout of `morpheus_analysis`, requires an ABI review.

ABI version 1 returns morphology values as fixed-width `uint32_t` fields. The
public constants in `<morpheus/morpheus.h>` are the contract for interpreting
person, number, gender, case, tense, mood, voice, degree, dialect, and
geographic region. The Deno binding exports the equivalent `Morpheus*`
constant objects.

Person, number, gender, case, voice, dialect, and geographic-region values are
bit masks. A result may therefore combine several applicable values; test bits
rather than assuming a single enumeration member. `MORPHEUS_DIALECT_ALL` is
zero and means that no dialect restriction is recorded. Tense and mood use the
named historical codes and must be compared for equality.

`stem_type` and `derivation_type` remain opaque stemlib codes in ABI version 1.
`morph_flags` remains the historical 96-bit compatibility array so that the
version-1 structure stays 860 bytes. `morpheus_result_all_morph_flags()` copies
the complete 112-bit in-memory representation. In particular, flag 110 is
published as `MORPHEUS_MORPH_FLAG_GROUP_NAME`; the stemlib stores that flag in
a legacy compatibility marker because its on-disk flag field remains 96 bits.
Flags 1–83 and 110 have named `MORPHEUS_MORPH_FLAG_*` constants. Flag numbers
are one-based: for flag `n`, inspect byte `(n - 1) / 8` and bit
`1 << ((n - 1) % 8)`. Values 84–109 and 111–112 are reserved.

## Fixed-capacity text

Each analysis text field is always NUL-terminated. The library copies at most
`capacity - 1` bytes and records every truncated field in a per-analysis mask.
Call `morpheus_result_truncated_fields()` before treating a copied value as
complete. Zero means that every text field was copied without truncation.

The mask is stored with the opaque result rather than in `morpheus_analysis`,
so the version-1 structure size and layout remain unchanged. The Deno binding
performs the query while it owns the native result and exposes the mask as
`MorpheusAnalysis.truncatedFields`; compare it with the
`MorpheusTruncatedField` constants.

## Per-request options

In addition to strict case, accent fallback, and verb-only analysis, callers may
disable crasis expansion, stop after the first successful dictionary class,
select the HQ dictionary backend, or restrict an analysis to one or more named
dialects. Dialect options are bit masks, may be combined, and are valid only
for Greek contexts. Passing one to a Latin or Italian context returns
`MORPHEUS_INVALID_ARGUMENT`.

All of these settings are scoped to one `morpheus_analyze()` call. The library
restores the context's previous crasis, quick-search, dictionary, and dialect
state before returning, including on allocation or input failure. Switching the
HQ backend may invalidate dictionary caches; repeatedly alternating it is valid
but more expensive than using a dedicated context for each backend.

The Deno binding exposes the same bits through `MorpheusOption`. They can be
combined with bigint bitwise OR and remain subject to the binding's per-context
serial queue.

For compatibility, `MorpheusAnalysis.morphFlags` remains 12 bytes. The binding
also exposes the complete 14-byte value as `allMorphFlags` and publishes flag
numbers through `MorpheusMorphFlag`. `hasMorpheusMorphFlag()` performs the
one-based byte-and-bit lookup safely.
