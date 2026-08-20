#include <assert.h>
#include <string.h>
#include <morpheus/morpheus.h>
int main(void)
{
  morpheus_context *context=NULL;
  char oversized_path[513];
  morpheus_config config={MORPHEUS_ABI_VERSION,sizeof config,"/tmp/morpheus-stemlib",MORPHEUS_LANGUAGE_GREEK};
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
  config.stemlib_path="/tmp/morpheus-stemlib";
  assert(strcmp(morpheus_status_message(MORPHEUS_ABI_MISMATCH),"ABI version mismatch")==0);
  assert(strcmp(morpheus_status_message((morpheus_status)99),"unknown status")==0);
  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(context);
  morpheus_close(context);
  return(0);
}
