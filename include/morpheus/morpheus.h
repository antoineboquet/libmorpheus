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
typedef struct { uint32_t abi_version; uint32_t struct_size; const char *stemlib_path; uint32_t language; } morpheus_config;
morpheus_status morpheus_open(const morpheus_config *config, morpheus_context **context);
void morpheus_close(morpheus_context *context);
#ifdef __cplusplus
}
#endif
#endif
