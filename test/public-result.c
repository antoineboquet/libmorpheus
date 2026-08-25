#include <assert.h>
#include <morpheus/morpheus.h>
int main(void)
{
  morpheus_analysis analysis;
  _Static_assert(MORPHEUS_MORPH_FLAG_GROUP_NAME <
                 MORPHEUS_MORPH_FLAG_CAPACITY*8u,
                 "public flag capacity covers group names");
  assert(morpheus_analysis_size()==sizeof analysis);
  assert(morpheus_result_count(NULL)==0);
  assert(morpheus_result_copy(NULL,0,&analysis,sizeof analysis)==
         MORPHEUS_INVALID_ARGUMENT);
  assert(morpheus_result_get(NULL,0,&analysis)==MORPHEUS_INVALID_ARGUMENT);
  assert(morpheus_result_truncated_fields(NULL,0,NULL)==
         MORPHEUS_INVALID_ARGUMENT);
  morpheus_result_free(NULL);
  return(0);
}
