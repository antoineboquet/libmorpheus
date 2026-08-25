// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <morpheus/compat.h>

#ifndef MORPHEUS_TEST_STEMLIB
#error MORPHEUS_TEST_STEMLIB is required
#endif

int
main(void)
{
  static const uint8_t word[]="kai\\";
  static const char expected[]=
      "kai\\\n<NL>N kai/,kai/ \t\tindeclform\tconj</NL>\n";
  morpheus_config config={
    MORPHEUS_ABI_VERSION,sizeof config,MORPHEUS_TEST_STEMLIB,
    MORPHEUS_LANGUAGE_GREEK
  };
  morpheus_context *context=NULL;
  morpheus_compat_output *output=NULL;
  const morpheus_compat_flags flags=
      MORPHEUS_COMPAT_PERSEUS_FORMAT|MORPHEUS_COMPAT_STRICT_CASE;

  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(morpheus_compat_analyze(context,word,sizeof word-1,flags,&output)==
         MORPHEUS_OK);
  assert(morpheus_compat_output_analysis_count(output)==1);
  assert(morpheus_compat_output_lemma_count(output)==1);
  assert(morpheus_compat_output_length(output)==strlen(expected));
  assert(strcmp(morpheus_compat_output_data(output),expected)==0);
  morpheus_compat_output_free(output);
  morpheus_close(context);
  return(0);
}
