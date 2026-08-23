#include <assert.h>

#include <gkstring.h>

#include "../src/anal/checkirreg.proto.h"
#include "../src/anal/checkdict.proto.h"
#include "../src/anal/dictstems.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"

int quickflag = 0;

static void
initialize_irregular_buffers(void)
{
	gk_word candidate = { 0 };

	set_stem(&candidate,"zzzzzzzz");
	assert(!try_irregvb(&candidate));
}

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	char *first_form;
	char *first_keys;

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(!try_irregvb(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!CheckIrregForm(NULL,NULL,NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!checkdict(NULL,NULL,NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(dictstems(NULL,NULL,NO,NULL,NULL,NULL,0) == -1);
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	initialize_irregular_buffers();
	assert(first->analysis_irregular_buffers_initialized);
	first_form = first->analysis_irregular_forms[0];
	first_keys = first->analysis_irregular_keys[0];

	morpheus_runtime_context_activate(second);
	assert(!second->analysis_irregular_buffers_initialized);
	initialize_irregular_buffers();
	assert(second->analysis_irregular_forms[0] != first_form);
	assert(second->analysis_irregular_keys[0] != first_keys);

	morpheus_runtime_context_activate(first);
	assert(first->analysis_irregular_forms[0] == first_form);
	assert(first->analysis_irregular_keys[0] == first_keys);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
