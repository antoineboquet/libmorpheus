#ifndef MORPHEUS_MORPHEUS_H
#define MORPHEUS_MORPHEUS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file morpheus.h
 * @brief Stable C ABI for the Morpheus morphological analyzer.
 *
 * Contexts and results are opaque. An analysis call owns its returned result
 * until morpheus_result_free() is called. Distinct contexts may be used
 * concurrently, but callers must serialize operations on any one context.
 */

/** ABI version implemented by this header. */
#define MORPHEUS_ABI_VERSION 1u

typedef struct morpheus_runtime_context morpheus_context;
typedef struct morpheus_result morpheus_result;
typedef struct morpheus_compat_output morpheus_compat_output;

/** Status returned by fallible public operations. */
typedef enum {
  MORPHEUS_OK=0,
  MORPHEUS_INVALID_ARGUMENT=1,
  MORPHEUS_ABI_MISMATCH=2,
  MORPHEUS_NO_MEMORY=3,
  MORPHEUS_INPUT_TOO_LONG=4,
  MORPHEUS_OUT_OF_RANGE=5,
  MORPHEUS_INTERNAL_ERROR=6,
  MORPHEUS_BUFFER_TOO_SMALL=7
} morpheus_status;

/** Runtime language selected when a context is created. */
typedef enum {
  MORPHEUS_LANGUAGE_GREEK=0,
  MORPHEUS_LANGUAGE_LATIN=1,
  MORPHEUS_LANGUAGE_ITALIAN=2
} morpheus_language;

#define MORPHEUS_TEXT_CAPACITY 64u
#define MORPHEUS_DOMAIN_CAPACITY 24u
#define MORPHEUS_MORPH_FLAG_CAPACITY 12u

/** Coarse part-of-speech classification exposed by ABI version 1. */
typedef enum {
  MORPHEUS_PART_OF_SPEECH_UNKNOWN=0,
  MORPHEUS_PART_OF_SPEECH_NOUN=1,
  MORPHEUS_PART_OF_SPEECH_VERB=2,
  MORPHEUS_PART_OF_SPEECH_ADJECTIVE=3
} morpheus_part_of_speech;

/**
 * One structured analysis.
 *
 * Numeric morphology fields preserve the runtime's stable integer codes.
 * Text fields are NUL-terminated, fixed-capacity Beta Code strings. The
 * struct_size member reports the size written by the producing library.
 */
typedef struct {
  uint32_t struct_size;
  uint32_t part_of_speech;
  uint32_t stem_type;
  uint32_t derivation_type;
  uint32_t dialect;
  uint32_t geographic_region;
  uint32_t person;
  uint32_t number;
  uint32_t gender;
  uint32_t grammatical_case;
  uint32_t tense;
  uint32_t mood;
  uint32_t voice;
  uint32_t degree;
  char raw[MORPHEUS_TEXT_CAPACITY];
  char workword[MORPHEUS_TEXT_CAPACITY];
  char lemma[MORPHEUS_TEXT_CAPACITY];
  char preverb[MORPHEUS_TEXT_CAPACITY];
  char augment[MORPHEUS_TEXT_CAPACITY];
  char stem[MORPHEUS_TEXT_CAPACITY];
  char suffix[MORPHEUS_TEXT_CAPACITY];
  char ending[MORPHEUS_TEXT_CAPACITY];
  char crasis[MORPHEUS_TEXT_CAPACITY];
  char dictionary_form[MORPHEUS_TEXT_CAPACITY];
  char english_form[MORPHEUS_TEXT_CAPACITY];
  char raw_preverb[MORPHEUS_TEXT_CAPACITY];
  char domains[MORPHEUS_DOMAIN_CAPACITY];
  uint8_t morph_flags[MORPHEUS_MORPH_FLAG_CAPACITY];
} morpheus_analysis;

/** Per-request structured-analysis options. */
typedef uint64_t morpheus_options;

#define MORPHEUS_OPTION_STRICT_CASE (UINT64_C(1) << 0)
#define MORPHEUS_OPTION_IGNORE_ACCENTS (UINT64_C(1) << 1)
#define MORPHEUS_OPTION_VERBS_ONLY (UINT64_C(1) << 2)

/**
 * Legacy formatter flags accepted by morpheus_compat_analyze().
 *
 * These values intentionally preserve the historical cruncher bit layout.
 */
typedef uint32_t morpheus_compat_flags;

#define MORPHEUS_COMPAT_SHOW_ANAL UINT32_C(01)
#define MORPHEUS_COMPAT_SHOW_LEMMA UINT32_C(02)
#define MORPHEUS_COMPAT_SHOW_MISSES UINT32_C(04)
#define MORPHEUS_COMPAT_BUFFER_ANALYSES UINT32_C(010)
#define MORPHEUS_COMPAT_CHECK_PREVERB UINT32_C(020)
#define MORPHEUS_COMPAT_KEEP_BETA UINT32_C(040)
#define MORPHEUS_COMPAT_SHOW_FULL_INFO UINT32_C(0100)
#define MORPHEUS_COMPAT_DATABASE_FORMAT UINT32_C(0200)
#define MORPHEUS_COMPAT_DATABASE_SHORT UINT32_C(0600)
#define MORPHEUS_COMPAT_STRICT_CASE UINT32_C(01000)
#define MORPHEUS_COMPAT_PARSE_FORMAT UINT32_C(02000)
#define MORPHEUS_COMPAT_PERSEUS_FORMAT UINT32_C(04000)
#define MORPHEUS_COMPAT_ENDING_INDEX UINT32_C(010000)
#define MORPHEUS_COMPAT_IGNORE_ACCENTS UINT32_C(020000)
#define MORPHEUS_COMPAT_LEXICON_OUTPUT UINT32_C(040000)
#define MORPHEUS_COMPAT_LEMMA_COUNT UINT32_C(0200000)
#define MORPHEUS_COMPAT_VERBS_ONLY UINT32_C(0400000)

/** Context creation parameters. */
typedef struct {
  uint32_t abi_version;
  uint32_t struct_size;
  const char *stemlib_path;
  uint32_t language;
} morpheus_config;

/** Return the ABI version implemented by the loaded library. */
uint32_t morpheus_abi_version(void);

/** Return the native size of morpheus_analysis for FFI callers. */
size_t morpheus_analysis_size(void);

/** Return a static, NUL-terminated description of @p status. */
const char *morpheus_status_message(morpheus_status status);

/**
 * Create a context from an explicit-length path.
 *
 * This entry point avoids embedding a native pointer in a configuration
 * structure and is therefore preferred by FFI bindings. The path must not
 * contain an embedded NUL. On success, the caller owns *context.
 */
morpheus_status morpheus_open_path(
    uint32_t abi_version, const uint8_t *stemlib_path,
    size_t stemlib_path_length, uint32_t language,
    morpheus_context **context);

/**
 * Create a context from a native C configuration.
 *
 * config->abi_version must equal MORPHEUS_ABI_VERSION and
 * config->struct_size must cover the complete version-1 structure.
 * On success, the caller owns *context.
 */
morpheus_status morpheus_open(
    const morpheus_config *config, morpheus_context **context);

/** Destroy a context. Passing NULL is allowed. */
void morpheus_close(morpheus_context *context);

/**
 * Analyze one Beta Code form.
 *
 * Input need not be NUL-terminated and must not contain an embedded NUL.
 * The result preserves the analyzer's order and duplicates. On success, the
 * caller owns *result, including when it contains zero analyses.
 */
morpheus_status morpheus_analyze(
    morpheus_context *context, const uint8_t *beta_code, size_t length,
    morpheus_options options, morpheus_result **result);

/** Return the number of analyses in a result, or zero for NULL. */
size_t morpheus_result_count(const morpheus_result *result);

/**
 * Copy one analysis into caller-owned storage.
 *
 * buffer_size must be at least morpheus_analysis_size(). This is the preferred
 * accessor for FFI callers because it does not expose an internal pointer.
 */
morpheus_status morpheus_result_copy(
    const morpheus_result *result, size_t index, void *buffer,
    size_t buffer_size);

/** Copy one analysis into a native morpheus_analysis structure. */
morpheus_status morpheus_result_get(
    const morpheus_result *result, size_t index,
    morpheus_analysis *analysis);

/** Destroy an owned result. Passing NULL is allowed. */
void morpheus_result_free(morpheus_result *result);

/**
 * Analyze and format one form using the historical cruncher representation.
 *
 * On success, the caller owns *output. The returned byte sequence is
 * NUL-terminated for convenience; its logical byte length is reported
 * separately and does not include that terminator.
 */
morpheus_status morpheus_compat_analyze(
    morpheus_context *context, const uint8_t *beta_code, size_t length,
    morpheus_compat_flags flags, morpheus_compat_output **output);

/** Borrow the NUL-terminated bytes owned by @p output. */
const char *morpheus_compat_output_data(
    const morpheus_compat_output *output);

/** Return the number of formatted bytes, excluding the NUL terminator. */
size_t morpheus_compat_output_length(
    const morpheus_compat_output *output);

/** Return the number of analyses found before formatting. */
size_t morpheus_compat_output_analysis_count(
    const morpheus_compat_output *output);

/** Return the number of distinct lemmas found before formatting. */
size_t morpheus_compat_output_lemma_count(
    const morpheus_compat_output *output);

/** Destroy an owned compatibility output. Passing NULL is allowed. */
void morpheus_compat_output_free(morpheus_compat_output *output);

#ifdef __cplusplus
}
#endif

#endif
