#include <morpheus/morpheus.h>
#include <stdlib.h>
#include <string.h>
#include "../morphlib/runtime_context_internal.h"
uint32_t morpheus_abi_version(void)
{
  return(MORPHEUS_ABI_VERSION);
}

const char *morpheus_status_message(morpheus_status status)
{
  switch(status) {
  case MORPHEUS_OK: return("success");
  case MORPHEUS_INVALID_ARGUMENT: return("invalid argument");
  case MORPHEUS_ABI_MISMATCH: return("ABI version mismatch");
  case MORPHEUS_NO_MEMORY: return("memory allocation failed");
  case MORPHEUS_INPUT_TOO_LONG: return("input is too long");
  case MORPHEUS_OUT_OF_RANGE: return("result index is out of range");
  case MORPHEUS_INTERNAL_ERROR: return("internal error");
  default: return("unknown status");
  }
}

static int runtime_language(uint32_t language)
{
  switch(language) {
  case MORPHEUS_LANGUAGE_GREEK: return(GREEK);
  case MORPHEUS_LANGUAGE_LATIN: return(LATIN);
  case MORPHEUS_LANGUAGE_ITALIAN: return(ITALIAN);
  default: return(-1);
  }
}
morpheus_status morpheus_open(const morpheus_config *config, morpheus_context **context)
{
  morpheus_runtime_context *runtime;
  size_t path_length;
  int language;
  if(!config || !context) return(MORPHEUS_INVALID_ARGUMENT);
  *context=NULL;
  if(config->abi_version != MORPHEUS_ABI_VERSION || config->struct_size < sizeof *config) return(MORPHEUS_ABI_MISMATCH);
  if(!config->stemlib_path || !config->stemlib_path[0]) return(MORPHEUS_INVALID_ARGUMENT);
  language=runtime_language(config->language);
  if(language < 0) return(MORPHEUS_INVALID_ARGUMENT);
  runtime=morpheus_runtime_context_create();
  if(!runtime) return(MORPHEUS_NO_MEMORY);
  path_length=strlen(config->stemlib_path);
  if(path_length >= MAXPATHNAME) {
    morpheus_runtime_context_destroy(runtime);
    return(MORPHEUS_INPUT_TOO_LONG);
  }
  runtime->stemlib_path=malloc(path_length+1);
  if(!runtime->stemlib_path) { morpheus_runtime_context_destroy(runtime); return(MORPHEUS_NO_MEMORY); }
  memcpy(runtime->stemlib_path,config->stemlib_path,path_length+1);
  morpheus_runtime_context_set_language(runtime,language);
  *context=runtime;
  return(MORPHEUS_OK);
}
void morpheus_close(morpheus_context *context) { morpheus_runtime_context_destroy(context); }
