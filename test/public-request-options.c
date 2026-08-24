#include <assert.h>
#include <morpheus/morpheus.h>

#include "../src/morphlib/runtime_context_internal.h"

#ifndef MORPHEUS_TEST_STEMLIB
#error MORPHEUS_TEST_STEMLIB is required
#endif

int main(void)
{
  static const uint8_t word[]="a)/nqrwpos";
  const morpheus_options scoped=
      MORPHEUS_OPTION_STRICT_CASE|MORPHEUS_OPTION_NO_CRASIS|
      MORPHEUS_OPTION_QUICK|MORPHEUS_OPTION_HQ_DICTIONARY|
      MORPHEUS_OPTION_DIALECT_ATTIC;
  morpheus_config config={
    MORPHEUS_ABI_VERSION,
    sizeof config,
    MORPHEUS_TEST_STEMLIB,
    MORPHEUS_LANGUAGE_GREEK
  };
  morpheus_context *context=NULL;
  morpheus_context *latin_context=NULL;
  morpheus_result *result=NULL;

  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(!context->analysis_crasis_disabled);
  assert(!context->analysis_quick_enabled);
  assert(!context->dictionary_hq_mode);
  assert(!context->analysis_wanted_dialects_initialized);
  assert(morpheus_analyze(context,word,sizeof word-1,scoped,&result)==
         MORPHEUS_STEMLIB_ERROR);
  assert(!result);
  assert(context->dictionary_hq_availability_checked);
  assert(!context->dictionary_hq_available);
  assert(morpheus_analyze(context,word,sizeof word-1,
                          MORPHEUS_OPTION_HQ_DICTIONARY,&result)==
         MORPHEUS_STEMLIB_ERROR);
  assert(!result);
  assert(!context->files_opened);
  assert(!context->analysis_crasis_disabled);
  assert(!context->analysis_quick_enabled);
  assert(!context->dictionary_hq_mode);
  assert(!context->analysis_wanted_dialects_initialized);

  assert(morpheus_analyze(context,word,sizeof word-1,
                          MORPHEUS_OPTION_STRICT_CASE,&result)==MORPHEUS_OK);
  assert(morpheus_result_count(result)>0);
  morpheus_result_free(result);
  result=NULL;

  assert(morpheus_analyze(context,word,sizeof word-1,UINT64_C(1)<<63,
                          &result)==MORPHEUS_INVALID_ARGUMENT);
  assert(!result);
  assert(morpheus_analyze(
             context,word,sizeof word-1,
             UINT64_C(1)<<MORPHEUS_OPTION_DIALECT_SHIFT,&result)==
         MORPHEUS_INVALID_ARGUMENT);
  assert(!result);
  config.language=MORPHEUS_LANGUAGE_LATIN;
  assert(morpheus_open(&config,&latin_context)==MORPHEUS_OK);
  assert(morpheus_analyze(latin_context,word,sizeof word-1,
                          MORPHEUS_OPTION_DIALECT_ATTIC,&result)==
         MORPHEUS_INVALID_ARGUMENT);
  assert(!result);
  morpheus_close(latin_context);
  morpheus_close(context);
  return(0);
}
