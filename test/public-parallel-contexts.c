#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <morpheus/morpheus.h>

#ifndef MORPHEUS_TEST_STEMLIB
#error MORPHEUS_TEST_STEMLIB is required
#endif

typedef struct {
  morpheus_context *context;
  const char *word;
  size_t expected_count;
  int failed;
} worker_input;

static void *
analyze_repeatedly(void *argument)
{
  worker_input *input=argument;
  size_t iteration;

  for(iteration=0;iteration<20;iteration++) {
    morpheus_result *result=NULL;
    morpheus_status status=morpheus_analyze(
        input->context,(const uint8_t *)input->word,strlen(input->word),
        MORPHEUS_OPTION_STRICT_CASE,&result);
    if(status != MORPHEUS_OK ||
       morpheus_result_count(result) != input->expected_count) {
      input->failed=1;
      morpheus_result_free(result);
      return(NULL);
    }
    morpheus_result_free(result);
  }
  return(NULL);
}

int
main(void)
{
  morpheus_config greek_config={
    MORPHEUS_ABI_VERSION,sizeof greek_config,MORPHEUS_TEST_STEMLIB,
    MORPHEUS_LANGUAGE_GREEK
  };
  morpheus_config latin_config={
    MORPHEUS_ABI_VERSION,sizeof latin_config,MORPHEUS_TEST_STEMLIB,
    MORPHEUS_LANGUAGE_LATIN
  };
  morpheus_context *greek_context=NULL;
  morpheus_context *latin_context=NULL;
  worker_input greek={NULL,"bi/ou",4,0};
  worker_input latin={NULL,"est",2,0};
  pthread_t greek_thread;
  pthread_t latin_thread;

  if(morpheus_open(&greek_config,&greek_context) != MORPHEUS_OK ||
     morpheus_open(&latin_config,&latin_context) != MORPHEUS_OK)
    return(1);
  greek.context=greek_context;
  latin.context=latin_context;
  if(pthread_create(&greek_thread,NULL,analyze_repeatedly,&greek) != 0 ||
     pthread_create(&latin_thread,NULL,analyze_repeatedly,&latin) != 0)
    return(1);
  if(pthread_join(greek_thread,NULL) != 0 ||
     pthread_join(latin_thread,NULL) != 0)
    return(1);

  morpheus_close(latin_context);
  morpheus_close(greek_context);
  return(greek.failed || latin.failed);
}
