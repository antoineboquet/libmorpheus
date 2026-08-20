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
  case MORPHEUS_BUFFER_TOO_SMALL: return("output buffer is too small");
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
morpheus_status morpheus_open_path(uint32_t abi_version, const uint8_t *stemlib_path, size_t stemlib_path_length, uint32_t language, morpheus_context **context)
{
  char path[MAXPATHNAME];
  morpheus_config config;

  if(!stemlib_path || !context) return(MORPHEUS_INVALID_ARGUMENT);
  if(!stemlib_path_length) return(MORPHEUS_INVALID_ARGUMENT);
  if(stemlib_path_length >= sizeof path) return(MORPHEUS_INPUT_TOO_LONG);
  if(memchr(stemlib_path,0,stemlib_path_length))
    return(MORPHEUS_INVALID_ARGUMENT);
  memcpy(path,stemlib_path,stemlib_path_length);
  path[stemlib_path_length]=0;
  config.abi_version=abi_version;
  config.struct_size=sizeof config;
  config.stemlib_path=path;
  config.language=language;
  return(morpheus_open(&config,context));
}

void morpheus_close(morpheus_context *context) { morpheus_runtime_context_destroy(context); }
