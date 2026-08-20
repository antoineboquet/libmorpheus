#include <assert.h>

#include <gkstring.h>

#include "../src/anal/checkhalf1.proto.h"
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

	assert(greek);
	assert(latin);
	previous = morpheus_runtime_context_activate(greek);
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
