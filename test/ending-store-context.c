#include <assert.h>

#include <gkstring.h>

#include "../src/gkends/stor.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	gk_string entry = { 0 };
	gk_string *first_store;

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(InitGstrMem());
	first_store = first->ending_store;
	set_gkstring(&entry,"alpha");
	assert(AddNewGstr(&entry) > 0);
	assert(first->ending_store_count == 1);
	assert(first->ending_store_max_string == 6);

	morpheus_runtime_context_activate(second);
	assert(InitGstrMem());
	assert(second->ending_store != first_store);
	assert(!second->ending_store_count);
	assert(!second->ending_store_max_string);

	morpheus_runtime_context_activate(first);
	assert(first->ending_store == first_store);
	assert(first->ending_store_count == 1);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
