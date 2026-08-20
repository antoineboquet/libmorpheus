#include <assert.h>

#include <gkstring.h>

#include "../src/anal/checkword.proto.h"
#include "../src/morphlib/runtime_context.h"

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(!get_quickflag());
	set_quickflag(1);
	assert(get_quickflag());

	morpheus_runtime_context_activate(second);
	assert(!get_quickflag());
	set_quickflag(0);
	assert(!get_quickflag());

	morpheus_runtime_context_activate(first);
	assert(get_quickflag());
	set_quickflag(0);
	assert(!get_quickflag());

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
