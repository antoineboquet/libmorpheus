#include <assert.h>

#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/setlang.proto.h"

#include <prntflags.h>

int main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(greek);
	assert(latin);
	assert(cur_lang() == GREEK);

	morpheus_runtime_context_set_language(greek,GREEK);
	morpheus_runtime_context_set_language(latin,LATIN);
	assert(morpheus_runtime_context_language(greek) == GREEK);
	assert(morpheus_runtime_context_language(latin) == LATIN);

	previous = morpheus_runtime_context_activate(latin);
	assert(cur_lang() == LATIN);
	set_lang(ITALIAN);
	assert(morpheus_runtime_context_language(latin) == ITALIAN);

	morpheus_runtime_context_activate(greek);
	assert(cur_lang() == GREEK);
	morpheus_runtime_context_destroy(greek);
	assert(cur_lang() == GREEK);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
