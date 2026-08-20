#include <assert.h>
#include <morpheus/morpheus.h>
int main(void)
{
  morpheus_context *context=NULL;
  morpheus_config config={MORPHEUS_ABI_VERSION,sizeof config,"/tmp/morpheus-stemlib",MORPHEUS_LANGUAGE_GREEK};
  assert(morpheus_open(NULL,&context)==MORPHEUS_INVALID_ARGUMENT);
  assert(morpheus_open(&config,NULL)==MORPHEUS_INVALID_ARGUMENT);
  config.abi_version++;
  assert(morpheus_open(&config,&context)==MORPHEUS_ABI_MISMATCH);
  config.abi_version=MORPHEUS_ABI_VERSION;
  config.language=99;
  assert(morpheus_open(&config,&context)==MORPHEUS_INVALID_ARGUMENT);
  config.language=MORPHEUS_LANGUAGE_GREEK;
  assert(morpheus_open(&config,&context)==MORPHEUS_OK);
  assert(context);
  morpheus_close(context);
  return(0);
}
