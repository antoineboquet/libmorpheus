#include "api_internal.h"
#include <stdlib.h>
#include <string.h>
size_t morpheus_analysis_size(void)
{
  return(sizeof(morpheus_analysis));
}

morpheus_result *morpheus_result_create(size_t count)
{
  morpheus_result *result=calloc(1,sizeof *result);
  if(!result) return(NULL);
  if(count) {
    result->analyses=calloc(count,sizeof *result->analyses);
    result->truncated_fields=calloc(count,sizeof *result->truncated_fields);
    result->all_morph_flags=calloc(
        count,MORPHEUS_ALL_MORPH_FLAG_CAPACITY);
    if(!result->analyses || !result->truncated_fields ||
       !result->all_morph_flags) {
      free(result->all_morph_flags);
      free(result->truncated_fields);
      free(result->analyses);
      free(result);
      return(NULL);
    }
  }
  result->count=count;
  return(result);
}
size_t morpheus_result_count(const morpheus_result *result) { return(result ? result->count : 0); }
morpheus_status morpheus_result_copy(const morpheus_result *result, size_t index, void *buffer, size_t buffer_size)
{
  if(!result || !buffer) return(MORPHEUS_INVALID_ARGUMENT);
  if(index >= result->count) return(MORPHEUS_OUT_OF_RANGE);
  if(buffer_size < sizeof(morpheus_analysis))
    return(MORPHEUS_BUFFER_TOO_SMALL);
  memcpy(buffer,&result->analyses[index],sizeof(morpheus_analysis));
  return(MORPHEUS_OK);
}

morpheus_status morpheus_result_get(const morpheus_result *result, size_t index, morpheus_analysis *analysis)
{
  return(morpheus_result_copy(result,index,analysis,sizeof *analysis));
}
morpheus_status morpheus_result_truncated_fields(
    const morpheus_result *result, size_t index,
    morpheus_truncated_fields *fields)
{
  if(!result || !fields) return(MORPHEUS_INVALID_ARGUMENT);
  if(index >= result->count) return(MORPHEUS_OUT_OF_RANGE);
  *fields=result->truncated_fields[index];
  return(MORPHEUS_OK);
}
morpheus_status morpheus_result_all_morph_flags(
    const morpheus_result *result, size_t index, uint8_t *buffer,
    size_t buffer_size)
{
  if(!result || !buffer) return(MORPHEUS_INVALID_ARGUMENT);
  if(index >= result->count) return(MORPHEUS_OUT_OF_RANGE);
  if(buffer_size < MORPHEUS_ALL_MORPH_FLAG_CAPACITY)
    return(MORPHEUS_BUFFER_TOO_SMALL);
  memcpy(buffer,result->all_morph_flags+
      index*MORPHEUS_ALL_MORPH_FLAG_CAPACITY,
      MORPHEUS_ALL_MORPH_FLAG_CAPACITY);
  return(MORPHEUS_OK);
}
void morpheus_result_free(morpheus_result *result)
{
  if(!result) return;
  free(result->all_morph_flags);
  free(result->truncated_fields);
  free(result->analyses);
  free(result);
}
