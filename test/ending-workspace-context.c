#include <assert.h>

#include <gkstring.h>

#include "../src/gkends/mkend.proto.h"
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
	gk_string avoid = { 0 };

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(InitGstrMem());
	set_gkstring(&generated,"zz");
	mk_end(gkstring_of(&generated),&generated,&avoid);
	assert(first->ending_store_count > 0);

	morpheus_runtime_context_activate(second);
	assert(InitGstrMem());
	assert(!second->ending_store_count);
	set_gkstring(&generated,"zz");
	mk_end(gkstring_of(&generated),&generated,&avoid);
	assert(second->ending_store_count > 0);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
