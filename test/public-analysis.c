#include <assert.h>
#include <string.h>
#include <morpheus/morpheus.h>
#ifndef MORPHEUS_TEST_STEMLIB
#error MORPHEUS_TEST_STEMLIB is required
#endif
int main(void)
{
  static const uint8_t word[]="a)/nqrwpos";
  morpheus_config config={MORPHEUS_ABI_VERSION,sizeof config,MORPHEUS_TEST_STEMLIB,MORPHEUS_LANGUAGE_GREEK};
  morpheus_context *context=NULL;
  morpheus_result *result=NULL;
  morpheus_analysis analysis;
  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(morpheus_analyze(context,word,sizeof word-1,0,&result)==MORPHEUS_OK);
  assert(morpheus_result_count(result)>0);
  assert(morpheus_result_get(result,0,&analysis)==MORPHEUS_OK);
  assert(analysis.struct_size==sizeof analysis);
  assert(analysis.raw[0] && analysis.lemma[0]);
  morpheus_result_free(result);
  morpheus_close(context);
  return(0);
}
