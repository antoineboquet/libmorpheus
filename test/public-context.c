// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <string.h>
#include <morpheus/morpheus.h>
int main(void)
{
  morpheus_context *context=NULL;
  assert(morpheus_abi_version()==MORPHEUS_ABI_VERSION);
  char oversized_path[513];
  morpheus_config config={MORPHEUS_ABI_VERSION,sizeof config,MORPHEUS_TEST_STEMLIB,MORPHEUS_LANGUAGE_GREEK};
  assert(morpheus_open(NULL,&context)==MORPHEUS_INVALID_ARGUMENT);
  assert(morpheus_open(&config,NULL)==MORPHEUS_INVALID_ARGUMENT);
  config.abi_version++;
  assert(morpheus_open(&config,&context)==MORPHEUS_ABI_MISMATCH);
  config.abi_version=MORPHEUS_ABI_VERSION;
  config.language=99;
  assert(morpheus_open(&config,&context)==MORPHEUS_INVALID_ARGUMENT);
  config.language=MORPHEUS_LANGUAGE_GREEK;
  memset(oversized_path,'x',sizeof oversized_path-1);
  oversized_path[sizeof oversized_path-1]=0;
  config.stemlib_path=oversized_path;
  assert(morpheus_open(&config,&context)==MORPHEUS_INPUT_TOO_LONG);
  config.stemlib_path=MORPHEUS_TEST_STEMLIB "/missing";
  assert(morpheus_open(&config,&context)==MORPHEUS_STEMLIB_ERROR);
  assert(context==NULL);
  assert(strcmp(morpheus_status_message(MORPHEUS_STEMLIB_ERROR),
                "stemlib is unavailable or incomplete")==0);
  config.stemlib_path=MORPHEUS_TEST_STEMLIB;
  assert(strcmp(morpheus_status_message(MORPHEUS_ABI_MISMATCH),"ABI version mismatch")==0);
  assert(strcmp(morpheus_status_message((morpheus_status)99),"unknown status")==0);
  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(context);
  morpheus_close(context);
  context=NULL;
  assert(morpheus_open_path(MORPHEUS_ABI_VERSION,
                            (const uint8_t *)config.stemlib_path,
                            strlen(config.stemlib_path),
                            MORPHEUS_LANGUAGE_GREEK,&context)==MORPHEUS_OK);
  morpheus_close(context);
  context=NULL;
  config.stemlib_path=MORPHEUS_TEST_STEMLIB "/missing";
  assert(morpheus_open_path(MORPHEUS_ABI_VERSION,
                            (const uint8_t *)config.stemlib_path,
                            strlen(config.stemlib_path),
                            MORPHEUS_LANGUAGE_GREEK,
                            &context)==MORPHEUS_STEMLIB_ERROR);
  assert(context==NULL);
  config.stemlib_path=MORPHEUS_TEST_STEMLIB;
  config.language=MORPHEUS_LANGUAGE_LATIN;
  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(context);
  morpheus_close(context);
  return(0);
}
