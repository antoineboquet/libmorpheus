#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <morpheus/morpheus.h>

#ifndef MORPHEUS_TEST_STEMLIB
#error MORPHEUS_TEST_STEMLIB is required
#endif

static size_t
analysis_count(morpheus_context *context, const char *word,
               morpheus_options options)
{
  morpheus_result *result = NULL;
  size_t count;

  assert(morpheus_analyze(context,(const uint8_t *)word,strlen(word),
                          options,&result) == MORPHEUS_OK);
  count=morpheus_result_count(result);
  morpheus_result_free(result);
  return(count);
}

int
main(void)
{
  morpheus_config config = {
    MORPHEUS_ABI_VERSION,
    sizeof config,
    MORPHEUS_TEST_STEMLIB,
    MORPHEUS_LANGUAGE_GREEK
  };
  morpheus_context *context = NULL;
  const morpheus_options strict=MORPHEUS_OPTION_STRICT_CASE;

  assert(morpheus_open(&config,&context) == MORPHEUS_OK);
  assert(analysis_count(context,"bi/ou",strict) == 4);
  assert(analysis_count(context,"bi/ou",
                        strict|MORPHEUS_OPTION_IGNORE_ACCENTS) == 8);
  assert(analysis_count(context,"tou=",strict) == 3);
  assert(analysis_count(context,"anqrwpos",
                        strict|MORPHEUS_OPTION_IGNORE_ACCENTS) > 0);
  morpheus_close(context);

  config.language=MORPHEUS_LANGUAGE_LATIN;
  assert(morpheus_open(&config,&context) == MORPHEUS_OK);
  assert(analysis_count(context,"est",strict) == 2);
  morpheus_close(context);
  return(0);
}
