#ifndef MORPHEUS_MORPHEUS_H
#define MORPHEUS_MORPHEUS_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define MORPHEUS_ABI_VERSION 1u
typedef struct morpheus_runtime_context morpheus_context;
typedef struct morpheus_result morpheus_result;
typedef enum { MORPHEUS_OK=0, MORPHEUS_INVALID_ARGUMENT=1, MORPHEUS_ABI_MISMATCH=2, MORPHEUS_NO_MEMORY=3, MORPHEUS_INPUT_TOO_LONG=4, MORPHEUS_OUT_OF_RANGE=5, MORPHEUS_INTERNAL_ERROR=6 } morpheus_status;
typedef enum { MORPHEUS_LANGUAGE_GREEK=0, MORPHEUS_LANGUAGE_LATIN=1, MORPHEUS_LANGUAGE_ITALIAN=2 } morpheus_language;
#define MORPHEUS_TEXT_CAPACITY 64u
#define MORPHEUS_DOMAIN_CAPACITY 24u
#define MORPHEUS_MORPH_FLAG_CAPACITY 12u
typedef enum { MORPHEUS_PART_OF_SPEECH_UNKNOWN=0, MORPHEUS_PART_OF_SPEECH_NOUN=1, MORPHEUS_PART_OF_SPEECH_VERB=2, MORPHEUS_PART_OF_SPEECH_ADJECTIVE=3 } morpheus_part_of_speech;
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
typedef uint64_t morpheus_options;
#define MORPHEUS_OPTION_STRICT_CASE (UINT64_C(1) << 0)
#define MORPHEUS_OPTION_IGNORE_ACCENTS (UINT64_C(1) << 1)
#define MORPHEUS_OPTION_VERBS_ONLY (UINT64_C(1) << 2)
typedef struct { uint32_t abi_version; uint32_t struct_size; const char *stemlib_path; uint32_t language; } morpheus_config;
uint32_t morpheus_abi_version(void);
size_t morpheus_analysis_size(void);
const char *morpheus_status_message(morpheus_status status);
morpheus_status morpheus_open(const morpheus_config *config, morpheus_context **context);
void morpheus_close(morpheus_context *context);
morpheus_status morpheus_analyze(morpheus_context *context, const uint8_t *beta_code, size_t length, morpheus_options options, morpheus_result **result);
size_t morpheus_result_count(const morpheus_result *result);
morpheus_status morpheus_result_get(const morpheus_result *result, size_t index, morpheus_analysis *analysis);
void morpheus_result_free(morpheus_result *result);
#ifdef __cplusplus
}
#endif
#endif
