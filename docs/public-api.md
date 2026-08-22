# Public analysis values

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
`morph_flags` remains the historical 96-bit array. Applications should preserve
these values but must not invent mappings for them until named public constants
are added.

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
