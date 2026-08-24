#ifndef MORPHEUS_API_INTERNAL_H
#define MORPHEUS_API_INTERNAL_H
#include <morpheus/morpheus.h>
#include "../morphlib/runtime_context.h"
struct morpheus_compat_output {
  char *data;
  size_t length;
  size_t analysis_count;
  size_t lemma_count;
};
struct morpheus_result {
  size_t count;
  morpheus_analysis *analyses;
  morpheus_truncated_fields *truncated_fields;
  uint8_t *all_morph_flags;
};
morpheus_result *morpheus_result_create(size_t count);
static inline morpheus_status morpheus_runtime_status(
    const morpheus_runtime_context *context)
{
  switch(morpheus_runtime_context_error(context)) {
  case MORPHEUS_RUNTIME_ERROR_NO_MEMORY: return(MORPHEUS_NO_MEMORY);
  case MORPHEUS_RUNTIME_ERROR_INTERNAL: return(MORPHEUS_INTERNAL_ERROR);
  default: return(MORPHEUS_OK);
  }
}
#endif
