# Public C API

`<morpheus/morpheus.h>` is the complete supported C surface. The shared
library hides every other symbol. The ABI is C-compatible from C++ through the
header's `extern "C"` block.

## Versioning and compatibility

The current project version is 0.1.0, the SONAME major is 0, and
`MORPHEUS_ABI_VERSION` is 1. Call `morpheus_abi_version()` after dynamically
loading the library and compare it with the header constant before using any
layout-dependent operation. `morpheus_analysis_size()` provides the producing
library's native structure size and is especially useful at an FFI boundary.

Within ABI version 1, existing function signatures, numeric constants, and the
layout of `morpheus_analysis` are stable. Adding an exported function is
compatible. Removing or changing one, reassigning a published constant, or
changing the structure layout requires an ABI review and normally a new ABI
version. The binary symbol test rejects unlisted exports.

## Ownership and concurrency

- A successful `morpheus_open()` or `morpheus_open_path()` returns an owned,
  opaque context. Release it with `morpheus_close()`.
- A successful `morpheus_analyze()` returns an owned result even when its count
  is zero. Release it with `morpheus_result_free()`.
- A successful `morpheus_compat_analyze()` returns an owned formatted output.
  Release it with `morpheus_compat_output_free()`.
- Accessor pointers are borrowed and remain valid only while their owner is
  alive. Never free `morpheus_status_message()` or
  `morpheus_compat_output_data()`.
- Distinct contexts may be used concurrently. Calls using the same context,
  including destruction, must be serialized by the caller. Results are
  independent of their originating context after a successful call, but must
  not be freed concurrently with an accessor operation.
- The `*_free(NULL)` and `morpheus_close(NULL)` operations are no-ops.

## Status values

| Status | Meaning |
| --- | --- |
| `MORPHEUS_OK` | The operation completed and any documented output ownership was transferred. |
| `MORPHEUS_INVALID_ARGUMENT` | A pointer, language, option combination, empty path, or embedded-NUL input is invalid. |
| `MORPHEUS_ABI_MISMATCH` | The requested ABI version or configuration size is incompatible. |
| `MORPHEUS_NO_MEMORY` | Allocation or compatibility-stream creation failed. |
| `MORPHEUS_INPUT_TOO_LONG` | A Beta Code input or constructed stemlib path exceeds its runtime capacity. |
| `MORPHEUS_OUT_OF_RANGE` | A result index is outside `[0, morpheus_result_count())`. |
| `MORPHEUS_INTERNAL_ERROR` | The runtime rejected an inconsistent or overflowing internal operation. |
| `MORPHEUS_BUFFER_TOO_SMALL` | Caller-provided result or flag storage is below the documented size. |
| `MORPHEUS_STEMLIB_ERROR` | Required language data is missing, unreadable, or empty. |

`morpheus_status_message()` maps any status to static diagnostic text. It is
for diagnostics rather than program control; callers should branch on the
numeric status. Fallible calls transfer no ownership unless they return
`MORPHEUS_OK`.

## Function reference

| Function | Contract |
| --- | --- |
| `morpheus_abi_version()` | Returns the ABI version implemented by the loaded library. |
| `morpheus_analysis_size()` | Returns the native size required by `morpheus_result_copy()`. |
| `morpheus_status_message()` | Returns borrowed static text for a status, including unknown values. |
| `morpheus_open()` | Creates a validated context from a native `morpheus_config`. |
| `morpheus_open_path()` | Creates a context from an explicit-length path without placing a pointer in an FFI configuration struct. |
| `morpheus_close()` | Destroys a context and all of its caches. |
| `morpheus_analyze()` | Analyzes one explicit-length Beta Code form with per-request options. |
| `morpheus_result_count()` | Returns the analysis count, or zero for `NULL`. |
| `morpheus_result_copy()` | Copies an analysis into opaque caller storage of at least `morpheus_analysis_size()` bytes. |
| `morpheus_result_get()` | Copies an analysis into a native `morpheus_analysis`. |
| `morpheus_result_truncated_fields()` | Returns the fixed-text fields truncated during result construction. |
| `morpheus_result_all_morph_flags()` | Copies the complete 14-byte morphology bitset. |
| `morpheus_result_free()` | Destroys an owned structured result. |
| `morpheus_compat_analyze()` | Produces the historical `cruncher` representation and its counts. |
| `morpheus_compat_output_data()` | Returns borrowed NUL-terminated formatted bytes, or `NULL` for `NULL`. |
| `morpheus_compat_output_length()` | Returns the formatted byte count excluding the terminator, or zero for `NULL`. |
| `morpheus_compat_output_analysis_count()` | Returns the pre-formatting analysis count, or zero for `NULL`. |
| `morpheus_compat_output_lemma_count()` | Returns the distinct-lemma count, or zero for `NULL`. |
| `morpheus_compat_output_free()` | Destroys an owned compatibility output. |

Inputs to both analysis functions are explicit-length byte sequences. They need
not be NUL-terminated, but an embedded NUL is invalid. The maximum accepted
length is intentionally an implementation capacity rather than a public
constant; callers must handle `MORPHEUS_INPUT_TOO_LONG`.

## Structured analysis values

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
published as `MORPHEUS_MORPH_FLAG_GROUP_NAME`; ending-table records store that
flag in a legacy compatibility marker because their on-disk flag field remains
96 bits. The precompiled Alpheios stem index predates this conversion and omits
the `is_group` annotations present in its source files. Analyses made from that
index therefore cannot report `GROUP_NAME` until the stem index is rebuilt from
those sources with a format that preserves the annotation.
Flags 1–83 and 110 have named `MORPHEUS_MORPH_FLAG_*` constants. Flag numbers
are one-based: for flag `n`, inspect byte `(n - 1) / 8` and bit
`1 << ((n - 1) % 8)`. Values 84–109 and 111–112 are reserved.

## Stemlib validation

Context creation checks that the selected language contains the required rule,
dictionary, ending, and derivation index files and that each is readable and
nonempty. A missing or incomplete layout returns `MORPHEUS_STEMLIB_ERROR`
without creating a context. Excessively long constructed paths return
`MORPHEUS_INPUT_TOO_LONG`.

The manifest is language-specific. In particular, the Greek principal-part
table is not required for the historical Latin stemlib, which does not contain
that file.

The Deno binding exports the same numeric values through `MorpheusStatus` and
reports them in `MorpheusError.status`.

This check detects deployment and language-selection mistakes; it does not
fully validate the historical binary formats. The differential fixtures remain
the integrity check for the contents of a distributed stemlib.

## Fixed-capacity text

Each analysis text field is always NUL-terminated. The library copies at most
`capacity - 1` bytes and records every truncated field in a per-analysis mask.
Call `morpheus_result_truncated_fields()` before treating a copied value as
complete. Zero means that every text field was copied without truncation.

The mask is stored with the opaque result rather than in `morpheus_analysis`,
so the version-1 structure size and layout remain unchanged. The Deno binding
performs the query while it owns the native result.
`MorpheusAnalysis.truncatedFields` is an array of stable field names;
`MorpheusRawAnalysis.truncatedFields` preserves the numeric mask for ABI-level
tools and may be compared with the `MorpheusTruncatedField` constants.

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

| Option | Effect |
| --- | --- |
| `MORPHEUS_OPTION_STRICT_CASE` | Requires the requested capitalization path. |
| `MORPHEUS_OPTION_IGNORE_ACCENTS` | Enables accent-insensitive fallback. |
| `MORPHEUS_OPTION_VERBS_ONLY` | Restricts the historical analyzer to verbal results. |
| `MORPHEUS_OPTION_NO_CRASIS` | Disables crasis expansion for this request. |
| `MORPHEUS_OPTION_QUICK` | Stops after the first successful dictionary class. |
| `MORPHEUS_OPTION_HQ_DICTIONARY` | Selects the HQ dictionary backend for this request. |
| `MORPHEUS_OPTION_DIALECT_*` | Combines one or more Greek dialect restrictions. |

Unknown option bits are invalid. Dialect restrictions on Latin or Italian
contexts return `MORPHEUS_INVALID_ARGUMENT`. Request state is restored before
returning on success or failure.

The ABI structure retains its 12-byte morphology flag field, while the result
accessor exposes all 14 bytes. The Deno `analyze()` method decodes these bits
as named `MorpheusAnalysis.morphFlags`. Its `analyzeRaw()` counterpart exposes
both byte vectors through `MorpheusRawAnalysis.morphFlags` and
`allMorphFlags`. `hasMorpheusMorphFlag()` accepts either representation and
performs the one-based byte-and-bit lookup safely for raw values.

## Compatibility output

The compatibility API exists for `cruncher` behavior and migration testing.
Its flags preserve the historical octal bit layout and its text is not a new
structured interchange format. New integrations should use
`morpheus_analyze()` and the result accessors. The returned data contains a
convenience NUL terminator; `morpheus_compat_output_length()` excludes it.
