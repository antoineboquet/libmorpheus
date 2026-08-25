# Public C API

`<morpheus/morpheus.h>` defines the normalized structured API. Historical
`cruncher` formatting is deliberately isolated in `<morpheus/compat.h>`.
The shared library hides every other symbol, and both headers are C-compatible
from C++.

## Versioning

The current project version is 0.2.0, the SONAME major is 1, and
`MORPHEUS_ABI_VERSION` is 2. This release candidate intentionally breaks ABI 1:
it removes stemlib-specific codes, assigns independent public morphology
values, replaces the historical flag storage with normalized public traits,
and removes the redundant complete-legacy-bitset accessor.

Call `morpheus_abi_version()` after dynamically loading the library and compare
it with the header constant. `morpheus_analysis_size()` returns the producing
library's native record size for FFI consumers. Within ABI 2, removing or
changing a function, reassigning a constant, or changing the record layout
requires another ABI review.

## Ownership and concurrency

- `morpheus_open()` and `morpheus_open_path()` return owned opaque contexts;
  release them with `morpheus_close()`.
- `morpheus_analyze()` returns an owned result, including when its count is
  zero; release it with `morpheus_result_free()`.
- `morpheus_compat_analyze()` returns an owned compatibility output; release it
  with `morpheus_compat_output_free()`.
- Distinct contexts may be used concurrently. Calls on one context, including
  destruction, must be serialized.
- Results remain independent of their context after a successful call.
- All documented `*_free(NULL)` operations and `morpheus_close(NULL)` are
  no-ops.

## Status values

| Status | Meaning |
| --- | --- |
| `MORPHEUS_OK` | Success and documented ownership transfer. |
| `MORPHEUS_INVALID_ARGUMENT` | Invalid pointer, input, language, or option combination. |
| `MORPHEUS_ABI_MISMATCH` | Header ABI or configuration size mismatch. |
| `MORPHEUS_NO_MEMORY` | Allocation failed. |
| `MORPHEUS_INPUT_TOO_LONG` | Input or constructed stemlib path exceeds capacity. |
| `MORPHEUS_OUT_OF_RANGE` | Result index is invalid. |
| `MORPHEUS_INTERNAL_ERROR` | Internal state or arithmetic failure. |
| `MORPHEUS_BUFFER_TOO_SMALL` | Caller storage is too small. |
| `MORPHEUS_STEMLIB_ERROR` | Required language data is absent, unreadable, or empty. |

`morpheus_status_message()` returns borrowed static diagnostic text. Programs
should branch on the numeric status.

## Function reference

| Function | Contract |
| --- | --- |
| `morpheus_abi_version()` | Returns the loaded ABI version. |
| `morpheus_analysis_size()` | Returns the native analysis-record size. |
| `morpheus_status_message()` | Returns borrowed status text. |
| `morpheus_open()` | Creates a validated context from `morpheus_config`. |
| `morpheus_open_path()` | Creates a context from an explicit-length path. |
| `morpheus_close()` | Destroys a context and its caches. |
| `morpheus_analyze()` | Produces normalized structured analyses. |
| `morpheus_result_count()` | Returns the count, or zero for `NULL`. |
| `morpheus_result_copy()` | Copies a record into caller storage. |
| `morpheus_result_get()` | Copies a record into `morpheus_analysis`. |
| `morpheus_result_truncated_fields()` | Reports truncated fixed-text fields. |
| `morpheus_result_free()` | Destroys a structured result. |
| `morpheus_compat_analyze()` | Produces historical `cruncher` text and counts. |
| `morpheus_compat_output_data()` | Returns borrowed formatted bytes. |
| `morpheus_compat_output_length()` | Returns the formatted byte count. |
| `morpheus_compat_output_analysis_count()` | Returns the analysis count before formatting. |
| `morpheus_compat_output_lemma_count()` | Returns the distinct-lemma count. |
| `morpheus_compat_output_free()` | Destroys compatibility output. |

Analysis inputs are explicit-length byte sequences. They need not end in NUL,
but an embedded NUL is invalid.

## Normalized analysis values

ABI 2 exposes only public `uint32_t` values. Person, number, tense, mood, and
degree are equality-tested enumerations. Gender, case, voice, dialect, and
geographic region are public masks and may combine values. The bridge converts
between these values and the engine's historical encodings; equality between a
public constant and an internal constant is never part of the contract.

`MORPHEUS_DEGREE_NONE` distinguishes an inapplicable degree from
`MORPHEUS_DEGREE_POSITIVE`. Irregular comparative and superlative markers are
normalized by the native bridge before a result is returned.

The record does not expose stem-type or derivation-type numbers. The public
part-of-speech classification replaces the useful lexical portion of those
internal identifiers. No stemlib record number is a supported ABI value.

## Public traits

`morph_flags` is an 11-byte public bitset covering
`MORPHEUS_MORPH_FLAG_COUNT` named traits. Trait values are zero-based,
alphabetically ordered public indices. For trait `n`, inspect byte `n / 8` and
bit `1 << (n % 8)`. The bridge translates each selected historical flag into
this representation; raw engine bytes and sparse historical flag numbers are
not returned.

The Deno binding exposes the same indices through `MorpheusMorphFlag` and
returns either the public byte vector (`analyzeRaw()`) or stable names
(`analyze()`). `hasMorpheusMorphFlag()` accepts either representation or a
readonly array of analyses.

## Fixed-capacity text

Every text field is NUL-terminated. At most `capacity - 1` bytes are copied.
`morpheus_result_truncated_fields()` reports any truncated field; zero means all
text was complete. The Deno semantic API returns named truncated fields, while
the raw API preserves the mask.

## Request options

Options are scoped to one `morpheus_analyze()` call. State is restored before
returning on success or failure.

| Option | Effect |
| --- | --- |
| `MORPHEUS_OPTION_STRICT_CASE` | Requires the requested capitalization path. |
| `MORPHEUS_OPTION_IGNORE_ACCENTS` | Enables accent-insensitive fallback. |
| `MORPHEUS_OPTION_VERBS_ONLY` | Restricts results to verbal forms. |
| `MORPHEUS_OPTION_NO_CRASIS` | Disables crasis expansion. |
| `MORPHEUS_OPTION_QUICK` | Stops after the first successful dictionary class. |
| `MORPHEUS_OPTION_HQ_DICTIONARY` | Uses the optional HQ dictionary backend. |
| `MORPHEUS_OPTION_DIALECT_*` | Combines one or more public Greek dialect masks. |

Dialect options are converted to the internal mask by the bridge and are
invalid for Latin or Italian contexts. Unknown option bits are invalid.

## Stemlib validation

Context creation verifies the language-specific rule, dictionary, ending, and
derivation files before analysis. This catches deployment errors but does not
fully validate historical binary formats; differential fixtures remain the
integrity check for distributed data.

## Historical compatibility API

The compatibility API exists for `cruncher` behavior and migration testing.
Include `<morpheus/compat.h>` explicitly. Its formatter flags retain their
historical octal layout, and the implementation remains on the MPL side of the
licensing boundary. New integrations should include only
`<morpheus/morpheus.h>` and use structured results.
