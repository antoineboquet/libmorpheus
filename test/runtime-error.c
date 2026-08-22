#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../src/api/api_internal.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"
#include "../src/morphlib/morphstrcmp.proto.h"
#include "../src/morphlib/morphkeys.proto.h"
#include "../src/morphlib/retrentry.proto.h"

int main(void)
{
  morpheus_runtime_context *context=morpheus_runtime_context_create();
  morpheus_runtime_context *previous;
  static const char missing_stemlib[]="/morpheus-test-missing-stemlib";
  endtags tag={0};
  char keys[LONGSTRING]="not empty";
  int maxkeys=1;

  assert(context);
  previous=morpheus_runtime_context_activate(context);
  assert(morpheus_runtime_context_error(context)==
         MORPHEUS_RUNTIME_ERROR_NONE);
  assert(morpheus_runtime_status(context)==MORPHEUS_OK);

  morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
  morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  context->analysis_storage_error=1;
  morpheus_runtime_context_clear_error(context);
  assert(context->analysis_storage_error==0);
  morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
  assert(morpheus_runtime_status(context)==MORPHEUS_NO_MEMORY);

  morpheus_runtime_context_clear_error(context);
  context->stemlib_path=malloc(sizeof missing_stemlib);
  assert(context->stemlib_path);
  memcpy(context->stemlib_path,missing_stemlib,sizeof missing_stemlib);
  assert(!init_keys());
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);
  assert(!context->stem_type_arguments);
  assert(!context->derivation_type_arguments);
  assert(!context->domain_arguments);
  assert(!context->morph_key_table);
  assert(!context->morph_key_stem_count);
  assert(!context->morph_key_derivation_count);
  assert(!context->morph_key_domain_count);
  assert(!context->morph_key_count);
  assert(!context->morph_keys_initialized);

  morpheus_runtime_context_clear_error(context);
  assert(!init_preind(NULL,&maxkeys));
  assert(maxkeys==0);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  assert(!init_preind("missing",NULL));
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  assert(ChckPreIndex(NULL,"a",0,1,morphstrcmp)==-1);
  assert(morpheus_runtime_status(context)==MORPHEUS_OK);
  assert(ChckPreIndex(NULL,"a",1,1,morphstrcmp)==-1);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  assert(ChckPreIndex(&tag,NULL,1,1,morphstrcmp)==-1);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  assert(ChckPreIndex(&tag,"a",1,1,NULL)==-1);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  assert(!ChckFullIndex(NULL,keys,"missing",0,morphstrncmp));
  assert(!keys[0]);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  assert(!ChckFullIndex("a",NULL,"missing",0,morphstrncmp));
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  strcpy(keys,"not empty");
  assert(!ChckFullIndex("a",keys,NULL,0,morphstrncmp));
  assert(!keys[0]);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_clear_error(context);
  strcpy(keys,"not empty");
  assert(!ChckFullIndex("a",keys,"missing",0,NULL));
  assert(!keys[0]);
  assert(morpheus_runtime_status(context)==MORPHEUS_INTERNAL_ERROR);

  morpheus_runtime_context_activate(previous);
  morpheus_runtime_context_destroy(context);
  return(0);
}
