/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_COMPAT_H
#define MORPHEUS_COMPAT_H

#include <morpheus/morpheus.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct morpheus_compat_output morpheus_compat_output;

/** Historical formatter flags accepted by morpheus_compat_analyze(). */
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

MORPHEUS_API morpheus_status morpheus_compat_analyze(
    morpheus_context *context, const uint8_t *beta_code, size_t length,
    morpheus_compat_flags flags, morpheus_compat_output **output);
MORPHEUS_API const char *morpheus_compat_output_data(
    const morpheus_compat_output *output);
MORPHEUS_API size_t morpheus_compat_output_length(
    const morpheus_compat_output *output);
MORPHEUS_API size_t morpheus_compat_output_analysis_count(
    const morpheus_compat_output *output);
MORPHEUS_API size_t morpheus_compat_output_lemma_count(
    const morpheus_compat_output *output);
MORPHEUS_API void morpheus_compat_output_free(morpheus_compat_output *output);

#ifdef __cplusplus
}
#endif

#endif
