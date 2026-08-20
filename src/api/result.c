#include "api_internal.h"
#include <stdlib.h>
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
    if(!result->analyses) { free(result); return(NULL); }
  }
  result->count=count;
  return(result);
}
size_t morpheus_result_count(const morpheus_result *result) { return(result ? result->count : 0); }
morpheus_status morpheus_result_get(const morpheus_result *result, size_t index, morpheus_analysis *analysis)
{
  if(!result || !analysis) return(MORPHEUS_INVALID_ARGUMENT);
  if(index >= result->count) return(MORPHEUS_OUT_OF_RANGE);
  *analysis=result->analyses[index];
  return(MORPHEUS_OK);
}
void morpheus_result_free(morpheus_result *result)
{
  if(!result) return;
  free(result->analyses);
  free(result);
}
