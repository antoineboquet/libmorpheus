// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <string.h>

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
	gk_string generated = { 0 };

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(InitGstrMem());
	set_gkstring(&generated,"zz");
	assert(AddNewGstr(&generated) == 1);
	assert(first->ending_store_count == 1);
	assert(!strcmp(gkstring_of(first->ending_store),"zz"));

	morpheus_runtime_context_activate(second);
	assert(InitGstrMem());
	assert(!second->ending_store_count);
	set_gkstring(&generated,"zz");
	assert(AddNewGstr(&generated) == 1);
	assert(second->ending_store_count == 1);
	assert(!strcmp(gkstring_of(second->ending_store),"zz"));
	assert(first->ending_store_count == 1);
	assert(first->ending_store != second->ending_store);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
