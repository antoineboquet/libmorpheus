#ifndef MORPHEUS_MORPHEUS_H
#define MORPHEUS_MORPHEUS_H

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#  if defined(MORPHEUS_BUILDING_LIBRARY)
#    define MORPHEUS_API __declspec(dllexport)
#  else
#    define MORPHEUS_API __declspec(dllimport)
#  endif
#elif defined(__GNUC__) || defined(__clang__)
#  define MORPHEUS_API __attribute__((visibility("default")))
#else
#  define MORPHEUS_API
#endif

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
#define MORPHEUS_ALL_MORPH_FLAG_CAPACITY 14u

/** One-based morphology flag numbers used by the analysis bitsets. */
typedef uint32_t morpheus_morph_flag;
#define MORPHEUS_MORPH_FLAG_SYLL_AUGMENT 1u
#define MORPHEUS_MORPH_FLAG_COMP_ONLY 2u
#define MORPHEUS_MORPH_FLAG_ENCLITIC 3u
#define MORPHEUS_MORPH_FLAG_ITERATIVE 4u
#define MORPHEUS_MORPH_FLAG_SUFFIX_ACCENT 5u
#define MORPHEUS_MORPH_FLAG_STEM_ACCENT 6u
#define MORPHEUS_MORPH_FLAG_CONTRACTED 7u
#define MORPHEUS_MORPH_FLAG_PERSON_NAME 8u
#define MORPHEUS_MORPH_FLAG_ANTEPENULT_ACCENT 9u
#define MORPHEUS_MORPH_FLAG_IRREGULAR_SUPERLATIVE 10u
#define MORPHEUS_MORPH_FLAG_IRREGULAR_COMPARATIVE 11u
#define MORPHEUS_MORPH_FLAG_NO_COMPARISON 12u
#define MORPHEUS_MORPH_FLAG_SHORT_PENULT 13u
#define MORPHEUS_MORPH_FLAG_LONG_PENULT 14u
#define MORPHEUS_MORPH_FLAG_RECESSIVE_ACCENT 15u
#define MORPHEUS_MORPH_FLAG_ACCENT_OPTIONAL 16u
#define MORPHEUS_MORPH_FLAG_NEEDS_ACCENT 17u
#define MORPHEUS_MORPH_FLAG_RHO_ETA_IOTA_ALPHA 18u
#define MORPHEUS_MORPH_FLAG_NOT_IN_COMPOSITION 19u
#define MORPHEUS_MORPH_FLAG_HAS_PREVERB 20u
#define MORPHEUS_MORPH_FLAG_UNAUGMENTED 21u
#define MORPHEUS_MORPH_FLAG_DISSIMILATION 22u
#define MORPHEUS_MORPH_FLAG_PROCLITIC 23u
#define MORPHEUS_MORPH_FLAG_APOCOPE 24u
#define MORPHEUS_MORPH_FLAG_IRREGULAR_FORM 25u
#define MORPHEUS_MORPH_FLAG_HAS_AUGMENT 26u
#define MORPHEUS_MORPH_FLAG_QUANTITY_METATHESIS 27u
#define MORPHEUS_MORPH_FLAG_NU_MOVABLE 28u
#define MORPHEUS_MORPH_FLAG_INTERVOCALIC_S_TO_H 29u
#define MORPHEUS_MORPH_FLAG_PREVERB_AUGMENT 30u
#define MORPHEUS_MORPH_FLAG_POETIC 31u
#define MORPHEUS_MORPH_FLAG_UNCONTRACTED_STEM 32u
#define MORPHEUS_MORPH_FLAG_METATHESIS 33u
#define MORPHEUS_MORPH_FLAG_ELIDED_PREVERB 34u
#define MORPHEUS_MORPH_FLAG_INDECLINABLE_FORM 35u
#define MORPHEUS_MORPH_FLAG_ROOT_PREVERB 36u
#define MORPHEUS_MORPH_FLAG_DIMINUTIVE 37u
#define MORPHEUS_MORPH_FLAG_LATE 38u
#define MORPHEUS_MORPH_FLAG_RARE 39u
#define MORPHEUS_MORPH_FLAG_RAW_PREVERB 40u
#define MORPHEUS_MORPH_FLAG_EARLY 41u
#define MORPHEUS_MORPH_FLAG_SHORT_SUBJUNCTIVE 42u
#define MORPHEUS_MORPH_FLAG_UNASPIRATED_PREVERB 43u
#define MORPHEUS_MORPH_FLAG_REDUPLICATION 44u
#define MORPHEUS_MORPH_FLAG_UNCONTRACTED_ENDING 45u
#define MORPHEUS_MORPH_FLAG_DERIVATIVE 46u
#define MORPHEUS_MORPH_FLAG_ATTIC_REDUPLICATION 47u
#define MORPHEUS_MORPH_FLAG_NO_REDUPLICATION 48u
#define MORPHEUS_MORPH_FLAG_N_INFIX 49u
#define MORPHEUS_MORPH_FLAG_SYNCOPE 50u
#define MORPHEUS_MORPH_FLAG_IMPERSONAL 51u
#define MORPHEUS_MORPH_FLAG_NEEDS_ROUGH_BREATHING 52u
#define MORPHEUS_MORPH_FLAG_NO_CIRCUMFLEX 53u
#define MORPHEUS_MORPH_FLAG_CAUSAL 54u
#define MORPHEUS_MORPH_FLAG_INTRANSITIVE 55u
#define MORPHEUS_MORPH_FLAG_TMESIS 56u
#define MORPHEUS_MORPH_FLAG_RAW_SONANT 57u
#define MORPHEUS_MORPH_FLAG_PRODELISION 58u
#define MORPHEUS_MORPH_FLAG_FREQUENTATIVE 59u
#define MORPHEUS_MORPH_FLAG_LATER 60u
#define MORPHEUS_MORPH_FLAG_DOUBLE_AUGMENT 61u
#define MORPHEUS_MORPH_FLAG_DOUBLE_REDUPLICATION 62u
#define MORPHEUS_MORPH_FLAG_DESIDERATIVE 63u
#define MORPHEUS_MORPH_FLAG_PRESENT_REDUPLICATION 64u
#define MORPHEUS_MORPH_FLAG_ENDS_IN_DIGAMMA 65u
#define MORPHEUS_MORPH_FLAG_GEOGRAPHIC_NAME 66u
#define MORPHEUS_MORPH_FLAG_DOUBLED_CONSONANT 67u
#define MORPHEUS_MORPH_FLAG_IOTA_INTENSIVE 68u
#define MORPHEUS_MORPH_FLAG_LOST_ACCENT 69u
#define MORPHEUS_MORPH_FLAG_SIGMA_TO_CI 70u
#define MORPHEUS_MORPH_FLAG_SHORT_EIS 71u
#define MORPHEUS_MORPH_FLAG_PROS_TO_POTI 72u
#define MORPHEUS_MORPH_FLAG_META_TO_PEDA 73u
#define MORPHEUS_MORPH_FLAG_PROS_TO_PROTI 74u
#define MORPHEUS_MORPH_FLAG_UPO_TO_UPAI 75u
#define MORPHEUS_MORPH_FLAG_PARA_TO_PARAI 76u
#define MORPHEUS_MORPH_FLAG_UPER_TO_UPEIR 77u
#define MORPHEUS_MORPH_FLAG_EN_TO_ENI 78u
#define MORPHEUS_MORPH_FLAG_ALPHA_PRIVATIVE 79u
#define MORPHEUS_MORPH_FLAG_ALPHA_COPULATIVE 80u
#define MORPHEUS_MORPH_FLAG_METRICALLY_LONG 81u
#define MORPHEUS_MORPH_FLAG_DELTA_PREVERB 82u
#define MORPHEUS_MORPH_FLAG_TAU_PREVERB 83u
#define MORPHEUS_MORPH_FLAG_GROUP_NAME 110u

/** Coarse part-of-speech classification exposed by ABI version 1. */
typedef enum {
  MORPHEUS_PART_OF_SPEECH_UNKNOWN=0,
  MORPHEUS_PART_OF_SPEECH_NOUN=1,
  MORPHEUS_PART_OF_SPEECH_VERB=2,
  MORPHEUS_PART_OF_SPEECH_ADJECTIVE=3
} morpheus_part_of_speech;

/* Morphology codes. Values that are powers of two may be combined. */
typedef uint32_t morpheus_person;
#define MORPHEUS_PERSON_NONE UINT32_C(0)
#define MORPHEUS_PERSON_FIRST UINT32_C(1)
#define MORPHEUS_PERSON_SECOND UINT32_C(2)
#define MORPHEUS_PERSON_THIRD UINT32_C(4)

typedef uint32_t morpheus_number;
#define MORPHEUS_NUMBER_NONE UINT32_C(0)
#define MORPHEUS_NUMBER_SINGULAR UINT32_C(1)
#define MORPHEUS_NUMBER_DUAL UINT32_C(2)
#define MORPHEUS_NUMBER_PLURAL UINT32_C(4)

typedef uint32_t morpheus_gender;
#define MORPHEUS_GENDER_NONE UINT32_C(0)
#define MORPHEUS_GENDER_MASCULINE UINT32_C(1)
#define MORPHEUS_GENDER_FEMININE UINT32_C(2)
#define MORPHEUS_GENDER_NEUTER UINT32_C(4)
#define MORPHEUS_GENDER_ADVERBIAL UINT32_C(8)

typedef uint32_t morpheus_case;
#define MORPHEUS_CASE_NONE UINT32_C(0)
#define MORPHEUS_CASE_NOMINATIVE UINT32_C(1)
#define MORPHEUS_CASE_GENITIVE UINT32_C(2)
#define MORPHEUS_CASE_DATIVE UINT32_C(4)
#define MORPHEUS_CASE_ACCUSATIVE UINT32_C(8)
#define MORPHEUS_CASE_VOCATIVE UINT32_C(16)
#define MORPHEUS_CASE_ABLATIVE UINT32_C(32)

typedef uint32_t morpheus_tense;
#define MORPHEUS_TENSE_NONE UINT32_C(0)
#define MORPHEUS_TENSE_PRESENT UINT32_C(1)
#define MORPHEUS_TENSE_IMPERFECT UINT32_C(10)
#define MORPHEUS_TENSE_FUTURE UINT32_C(3)
#define MORPHEUS_TENSE_AORIST UINT32_C(12)
#define MORPHEUS_TENSE_PERFECT UINT32_C(5)
#define MORPHEUS_TENSE_PLUPERFECT UINT32_C(6)
#define MORPHEUS_TENSE_FUTURE_PERFECT UINT32_C(15)
#define MORPHEUS_TENSE_PAST_ABSOLUTE UINT32_C(8)

typedef uint32_t morpheus_mood;
#define MORPHEUS_MOOD_NONE UINT32_C(0)
#define MORPHEUS_MOOD_INDICATIVE UINT32_C(1)
#define MORPHEUS_MOOD_SUBJUNCTIVE UINT32_C(2)
#define MORPHEUS_MOOD_OPTATIVE UINT32_C(3)
#define MORPHEUS_MOOD_IMPERATIVE UINT32_C(4)
#define MORPHEUS_MOOD_INFINITIVE UINT32_C(5)
#define MORPHEUS_MOOD_PARTICIPLE UINT32_C(6)
#define MORPHEUS_MOOD_GERUNDIVE UINT32_C(7)
#define MORPHEUS_MOOD_SUPINE UINT32_C(8)
#define MORPHEUS_MOOD_CONDITIONAL UINT32_C(9)

typedef uint32_t morpheus_voice;
#define MORPHEUS_VOICE_NONE UINT32_C(0)
#define MORPHEUS_VOICE_ACTIVE UINT32_C(1)
#define MORPHEUS_VOICE_MIDDLE UINT32_C(2)
#define MORPHEUS_VOICE_PASSIVE UINT32_C(4)
#define MORPHEUS_VOICE_MEDIO_PASSIVE \
  (MORPHEUS_VOICE_MIDDLE|MORPHEUS_VOICE_PASSIVE)
#define MORPHEUS_VOICE_DEPONENT \
  (MORPHEUS_VOICE_MIDDLE|MORPHEUS_VOICE_ACTIVE)

typedef uint32_t morpheus_degree;
#define MORPHEUS_DEGREE_POSITIVE UINT32_C(0)
#define MORPHEUS_DEGREE_COMPARATIVE UINT32_C(1)
#define MORPHEUS_DEGREE_SUPERLATIVE UINT32_C(2)

typedef uint32_t morpheus_dialect;
#define MORPHEUS_DIALECT_ALL UINT32_C(0)
#define MORPHEUS_DIALECT_ATTIC UINT32_C(2)
#define MORPHEUS_DIALECT_IONIC UINT32_C(8)
#define MORPHEUS_DIALECT_AEOLIC UINT32_C(16)
#define MORPHEUS_DIALECT_LESBIAN UINT32_C(32)
#define MORPHEUS_DIALECT_HOMERIC UINT32_C(64)
#define MORPHEUS_DIALECT_DORIC UINT32_C(128)
#define MORPHEUS_DIALECT_PARADIGM UINT32_C(256)
#define MORPHEUS_DIALECT_NON_HOMERIC_EPIC UINT32_C(1024)
#define MORPHEUS_DIALECT_EPIC \
  (MORPHEUS_DIALECT_NON_HOMERIC_EPIC|MORPHEUS_DIALECT_HOMERIC)
#define MORPHEUS_DIALECT_PROSE UINT32_C(2048)

typedef uint32_t morpheus_geographic_region;
#define MORPHEUS_REGION_NONE UINT32_C(0)
#define MORPHEUS_REGION_PHOCIS UINT32_C(1)
#define MORPHEUS_REGION_LOCRIS UINT32_C(2)
#define MORPHEUS_REGION_ELIS UINT32_C(4)
#define MORPHEUS_REGION_LACONIA UINT32_C(16)
#define MORPHEUS_REGION_HERACLEA UINT32_C(32)
#define MORPHEUS_REGION_MEGARID UINT32_C(64)
#define MORPHEUS_REGION_ARGOLID UINT32_C(128)
#define MORPHEUS_REGION_RHODES UINT32_C(256)
#define MORPHEUS_REGION_COS UINT32_C(512)
#define MORPHEUS_REGION_THERA UINT32_C(1024)
#define MORPHEUS_REGION_CYRENE UINT32_C(2048)
#define MORPHEUS_REGION_CRETE UINT32_C(4096)
#define MORPHEUS_REGION_ARCADIA UINT32_C(8192)
#define MORPHEUS_REGION_CYPRUS UINT32_C(16384)
#define MORPHEUS_REGION_BOEOTIA UINT32_C(32768)

/**
 * One structured analysis.
 *
 * Numeric morphology fields use the public constants above; stem_type and
 * derivation_type remain opaque legacy codes. Text fields are NUL-terminated,
 * fixed-capacity Beta Code strings. Use morpheus_result_truncated_fields() to
 * detect any value that exceeded its destination. The struct_size member
 * reports the size written by the producing library.
 */
typedef struct {
  uint32_t struct_size;
  uint32_t part_of_speech;
  uint32_t stem_type;
  uint32_t derivation_type;
  morpheus_dialect dialect;
  morpheus_geographic_region geographic_region;
  morpheus_person person;
  morpheus_number number;
  morpheus_gender gender;
  morpheus_case grammatical_case;
  morpheus_tense tense;
  morpheus_mood mood;
  morpheus_voice voice;
  morpheus_degree degree;
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

/** Bit mask returned by morpheus_result_truncated_fields(). */
typedef uint32_t morpheus_truncated_fields;
#define MORPHEUS_TRUNCATED_RAW (UINT32_C(1) << 0)
#define MORPHEUS_TRUNCATED_WORKWORD (UINT32_C(1) << 1)
#define MORPHEUS_TRUNCATED_LEMMA (UINT32_C(1) << 2)
#define MORPHEUS_TRUNCATED_PREVERB (UINT32_C(1) << 3)
#define MORPHEUS_TRUNCATED_AUGMENT (UINT32_C(1) << 4)
#define MORPHEUS_TRUNCATED_STEM (UINT32_C(1) << 5)
#define MORPHEUS_TRUNCATED_SUFFIX (UINT32_C(1) << 6)
#define MORPHEUS_TRUNCATED_ENDING (UINT32_C(1) << 7)
#define MORPHEUS_TRUNCATED_CRASIS (UINT32_C(1) << 8)
#define MORPHEUS_TRUNCATED_DICTIONARY_FORM (UINT32_C(1) << 9)
#define MORPHEUS_TRUNCATED_ENGLISH_FORM (UINT32_C(1) << 10)
#define MORPHEUS_TRUNCATED_RAW_PREVERB (UINT32_C(1) << 11)
#define MORPHEUS_TRUNCATED_DOMAINS (UINT32_C(1) << 12)

/** Per-request structured-analysis options. */
typedef uint64_t morpheus_options;

#define MORPHEUS_OPTION_STRICT_CASE (UINT64_C(1) << 0)
#define MORPHEUS_OPTION_IGNORE_ACCENTS (UINT64_C(1) << 1)
#define MORPHEUS_OPTION_VERBS_ONLY (UINT64_C(1) << 2)
#define MORPHEUS_OPTION_NO_CRASIS (UINT64_C(1) << 3)
#define MORPHEUS_OPTION_QUICK (UINT64_C(1) << 4)
#define MORPHEUS_OPTION_HQ_DICTIONARY (UINT64_C(1) << 5)

/** Shift applied to public dialect bits embedded in morpheus_options. */
#define MORPHEUS_OPTION_DIALECT_SHIFT 16u
#define MORPHEUS_DIALECT_KNOWN_MASK \
  (MORPHEUS_DIALECT_ATTIC|MORPHEUS_DIALECT_IONIC| \
   MORPHEUS_DIALECT_AEOLIC|MORPHEUS_DIALECT_LESBIAN| \
   MORPHEUS_DIALECT_HOMERIC|MORPHEUS_DIALECT_DORIC| \
   MORPHEUS_DIALECT_PARADIGM|MORPHEUS_DIALECT_NON_HOMERIC_EPIC| \
   MORPHEUS_DIALECT_PROSE)
#define MORPHEUS_OPTION_DIALECT_MASK \
  ((morpheus_options)MORPHEUS_DIALECT_KNOWN_MASK << \
   MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_ATTIC \
  (UINT64_C(2) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_IONIC \
  (UINT64_C(8) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_AEOLIC \
  (UINT64_C(16) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_LESBIAN \
  (UINT64_C(32) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_HOMERIC \
  (UINT64_C(64) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_DORIC \
  (UINT64_C(128) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_PARADIGM \
  (UINT64_C(256) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_NON_HOMERIC_EPIC \
  (UINT64_C(1024) << MORPHEUS_OPTION_DIALECT_SHIFT)
#define MORPHEUS_OPTION_DIALECT_EPIC \
  (MORPHEUS_OPTION_DIALECT_HOMERIC| \
   MORPHEUS_OPTION_DIALECT_NON_HOMERIC_EPIC)
#define MORPHEUS_OPTION_DIALECT_PROSE \
  (UINT64_C(2048) << MORPHEUS_OPTION_DIALECT_SHIFT)

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
MORPHEUS_API uint32_t morpheus_abi_version(void);

/** Return the native size of morpheus_analysis for FFI callers. */
MORPHEUS_API size_t morpheus_analysis_size(void);

/** Return a static, NUL-terminated description of @p status. */
MORPHEUS_API const char *morpheus_status_message(morpheus_status status);

/**
 * Create a context from an explicit-length path.
 *
 * This entry point avoids embedding a native pointer in a configuration
 * structure and is therefore preferred by FFI bindings. The path must not
 * contain an embedded NUL. On success, the caller owns *context.
 */
MORPHEUS_API morpheus_status morpheus_open_path(
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
MORPHEUS_API morpheus_status morpheus_open(
    const morpheus_config *config, morpheus_context **context);

/** Destroy a context. Passing NULL is allowed. */
MORPHEUS_API void morpheus_close(morpheus_context *context);

/**
 * Analyze one Beta Code form.
 *
 * Input need not be NUL-terminated and must not contain an embedded NUL.
 * The result preserves the analyzer's order and duplicates. On success, the
 * caller owns *result, including when it contains zero analyses.
 */
MORPHEUS_API morpheus_status morpheus_analyze(
    morpheus_context *context, const uint8_t *beta_code, size_t length,
    morpheus_options options, morpheus_result **result);

/** Return the number of analyses in a result, or zero for NULL. */
MORPHEUS_API size_t morpheus_result_count(const morpheus_result *result);

/**
 * Copy one analysis into caller-owned storage.
 *
 * buffer_size must be at least morpheus_analysis_size(). This is the preferred
 * accessor for FFI callers because it does not expose an internal pointer.
 */
MORPHEUS_API morpheus_status morpheus_result_copy(
    const morpheus_result *result, size_t index, void *buffer,
    size_t buffer_size);

/** Copy one analysis into a native morpheus_analysis structure. */
MORPHEUS_API morpheus_status morpheus_result_get(
    const morpheus_result *result, size_t index,
    morpheus_analysis *analysis);

/**
 * Report which fixed-capacity text fields were truncated while copying one
 * analysis. Zero means every field was copied in full.
 */
MORPHEUS_API morpheus_status morpheus_result_truncated_fields(
    const morpheus_result *result, size_t index,
    morpheus_truncated_fields *fields);

/**
 * Copy the complete morphology flag bitset for one analysis.
 *
 * This includes flags beyond the 96-bit compatibility field embedded in
 * morpheus_analysis. buffer_size must be at least
 * MORPHEUS_ALL_MORPH_FLAG_CAPACITY.
 */
MORPHEUS_API morpheus_status morpheus_result_all_morph_flags(
    const morpheus_result *result, size_t index, uint8_t *buffer,
    size_t buffer_size);

/** Destroy an owned result. Passing NULL is allowed. */
MORPHEUS_API void morpheus_result_free(morpheus_result *result);

/**
 * Analyze and format one form using the historical cruncher representation.
 *
 * On success, the caller owns *output. The returned byte sequence is
 * NUL-terminated for convenience; its logical byte length is reported
 * separately and does not include that terminator.
 */
MORPHEUS_API morpheus_status morpheus_compat_analyze(
    morpheus_context *context, const uint8_t *beta_code, size_t length,
    morpheus_compat_flags flags, morpheus_compat_output **output);

/** Borrow the NUL-terminated bytes owned by @p output. */
MORPHEUS_API const char *morpheus_compat_output_data(
    const morpheus_compat_output *output);

/** Return the number of formatted bytes, excluding the NUL terminator. */
MORPHEUS_API size_t morpheus_compat_output_length(
    const morpheus_compat_output *output);

/** Return the number of analyses found before formatting. */
MORPHEUS_API size_t morpheus_compat_output_analysis_count(
    const morpheus_compat_output *output);

/** Return the number of distinct lemmas found before formatting. */
MORPHEUS_API size_t morpheus_compat_output_lemma_count(
    const morpheus_compat_output *output);

/** Destroy an owned compatibility output. Passing NULL is allowed. */
MORPHEUS_API void morpheus_compat_output_free(morpheus_compat_output *output);

#ifdef __cplusplus
}
#endif

#endif
