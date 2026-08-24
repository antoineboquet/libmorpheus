#include <assert.h>

#include <gkstring.h>

#include "../src/anal/checkhalf1.proto.h"
#include "../src/anal/checkstem.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"

int quickflag = 0;

static void
initialize_stem_buffers(void)
{
	gk_word candidate = { 0 };
	char no_ending_keys[] = "";

	set_stem(&candidate,"zzzzzzzz");
	assert(!checkhalf2(&candidate,no_ending_keys));
}

int
main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	gk_string *greek_augmented;
	gk_string *greek_possible;
	char no_ending_keys[] = "";

	assert(greek);
	assert(latin);
	previous = morpheus_runtime_context_activate(greek);
	assert(!checkhalf1(NULL,no_ending_keys));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	assert(!checkhalf2(NULL,no_ending_keys));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	assert(!StemWorks(NULL,no_ending_keys,(gk_string *)&(gk_string){ 0 }));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	assert(!checkstem(NULL,no_ending_keys,NULL,NULL,0));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	assert(!stemexists(NULL,no_ending_keys,no_ending_keys,NO));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	initialize_stem_buffers();
	assert(greek->analysis_augmented_stems_initialized);
	assert(greek->analysis_possible_stems_initialized);
	greek_augmented = greek->analysis_augmented_stems[0];
	greek_possible = greek->analysis_possible_stems[0];

	morpheus_runtime_context_activate(latin);
	assert(!latin->analysis_augmented_stems_initialized);
	assert(!latin->analysis_possible_stems_initialized);
	initialize_stem_buffers();
	assert(latin->analysis_augmented_stems[0] != greek_augmented);
	assert(latin->analysis_possible_stems[0] != greek_possible);

	morpheus_runtime_context_activate(greek);
	assert(greek->analysis_augmented_stems[0] == greek_augmented);
	assert(greek->analysis_possible_stems[0] == greek_possible);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
