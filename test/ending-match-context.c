#include <assert.h>

#include <gkstring.h>

#include "../src/gkends/retrends.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	char ending[MAXWORDSIZE];

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	setwendstr(ending,"ab-");
	assert(first->ending_start_match);
	assert(!endstrcmp(ending,"a|b"));

	morpheus_runtime_context_activate(second);
	assert(!second->ending_start_match);
	setwendstr(ending,"ab");
	assert(!second->ending_start_match);
	assert(endstrcmp(ending,"a|b"));

	morpheus_runtime_context_activate(first);
	assert(first->ending_start_match);
	assert(!endstrcmp(ending,"a|b"));

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
