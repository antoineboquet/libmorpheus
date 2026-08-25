// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <string.h>

#include <morpheus/morpheus.h>

#ifndef MORPHEUS_TEST_STEMLIB
#error MORPHEUS_TEST_STEMLIB is required
#endif

int
main(void)
{
  static const uint8_t word[] = "a)/nqrwpos";
  morpheus_config config = {
    MORPHEUS_ABI_VERSION,
    sizeof config,
    MORPHEUS_TEST_STEMLIB,
    MORPHEUS_LANGUAGE_GREEK
  };
  morpheus_context *first_context = NULL;
  morpheus_context *second_context = NULL;
  morpheus_result *first_result = NULL;
  morpheus_result *second_result = NULL;
  morpheus_result *later_result = NULL;
  morpheus_analysis before;
  morpheus_analysis after;

  assert(morpheus_open(&config,&first_context) == MORPHEUS_OK);
  assert(morpheus_open(&config,&second_context) == MORPHEUS_OK);
  assert(morpheus_analyze(first_context,word,sizeof word-1,0,&first_result) ==
         MORPHEUS_OK);
  assert(morpheus_result_count(first_result) > 0);
  assert(morpheus_result_get(first_result,0,&before) == MORPHEUS_OK);

  assert(morpheus_analyze(second_context,word,sizeof word-1,0,&second_result) ==
         MORPHEUS_OK);
  assert(morpheus_result_count(second_result) ==
         morpheus_result_count(first_result));
  morpheus_close(second_context);
  assert(morpheus_result_get(second_result,0,&after) == MORPHEUS_OK);
  assert(strcmp(before.raw,after.raw) == 0);
  assert(strcmp(before.lemma,after.lemma) == 0);

  assert(morpheus_analyze(first_context,word,sizeof word-1,
                          MORPHEUS_OPTION_IGNORE_ACCENTS,&later_result) ==
         MORPHEUS_OK);
  assert(morpheus_result_get(first_result,0,&after) == MORPHEUS_OK);
  assert(before.part_of_speech == after.part_of_speech);
  assert(before.part_of_speech == after.part_of_speech);
  assert(!strcmp(before.lemma,after.lemma));
  assert(strcmp(before.raw,after.raw) == 0);
  assert(strcmp(before.lemma,after.lemma) == 0);
  assert(strcmp(before.stem,after.stem) == 0);
  assert(strcmp(before.ending,after.ending) == 0);

  morpheus_result_free(later_result);
  morpheus_result_free(second_result);
  morpheus_result_free(first_result);
  morpheus_close(first_context);
  return(0);
}
