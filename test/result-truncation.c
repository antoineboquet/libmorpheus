#include <assert.h>
#include <morpheus/morpheus.h>

#include "../src/api/api_internal.h"

int main(void)
{
  const morpheus_truncated_fields expected=
      MORPHEUS_TRUNCATED_LEMMA|MORPHEUS_TRUNCATED_DOMAINS;
  morpheus_truncated_fields fields=0;
  morpheus_result *result=morpheus_result_create(1);

  assert(result);
  result->truncated_fields[0]=expected;
  assert(morpheus_result_truncated_fields(result,0,&fields)==MORPHEUS_OK);
  assert(fields==expected);
  assert(morpheus_result_truncated_fields(result,1,&fields)==
         MORPHEUS_OUT_OF_RANGE);
  assert(morpheus_result_truncated_fields(result,0,NULL)==
         MORPHEUS_INVALID_ARGUMENT);
  morpheus_result_free(result);
  return(0);
}
