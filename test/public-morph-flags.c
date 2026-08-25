#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <morpheus/morpheus.h>

#ifndef MORPHEUS_TEST_STEMLIB
#error MORPHEUS_TEST_STEMLIB is required
#endif

static int flag_is_set(
    const uint8_t flags[MORPHEUS_MORPH_FLAG_CAPACITY],
    morpheus_morph_flag flag)
{
  const size_t index=(size_t)flag/8u;
  const uint8_t mask=(uint8_t)(UINT8_C(1)<<(flag%8u));
  return(index < MORPHEUS_MORPH_FLAG_CAPACITY &&
         (flags[index]&mask) != 0);
}

int main(void)
{
  static const uint8_t word[]="*)arta/chs";
  morpheus_config config={
    MORPHEUS_ABI_VERSION,
    sizeof config,
    MORPHEUS_TEST_STEMLIB,
    MORPHEUS_LANGUAGE_GREEK
  };
  morpheus_context *context=NULL;
  morpheus_result *result=NULL;
  size_t index;
  int found=0;

  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(morpheus_analyze(context,word,sizeof word-1,
                          MORPHEUS_OPTION_STRICT_CASE,&result)==MORPHEUS_OK);
  for(index=0;index<morpheus_result_count(result);index++) {
    morpheus_analysis analysis;
    assert(morpheus_result_get(result,index,&analysis)==MORPHEUS_OK);
    if(flag_is_set(analysis.morph_flags,MORPHEUS_MORPH_FLAG_PERSON_NAME)) {
      assert(!strcmp(analysis.lemma,"*)arta/chs"));
      found=1;
    }
  }
  assert(found);
  morpheus_result_free(result);
  morpheus_close(context);
  return(0);
}
