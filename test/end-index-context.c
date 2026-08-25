// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>

#include <gkstring.h>

#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"
#include "../src/gkends/endindex.proto.h"

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	endind *first_index;
	char keys[LONGSTRING];

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(!chcknend("zzzzzzzz",keys));
	assert(first->nominal_ending_index);
	first_index = first->nominal_ending_index;
	assert(endlen_of(first_index) > 0);
	assert(endeptr_of(first_index)[endlen_of(first_index)-1]);
	assert(endeptr_of(first_index)[endlen_of(first_index)-1][0]);

	morpheus_runtime_context_activate(second);
	assert(!second->nominal_ending_index);
	assert(!chcknend("zzzzzzzz",keys));
	assert(second->nominal_ending_index != first_index);

	morpheus_runtime_context_activate(first);
	assert(first->nominal_ending_index == first_index);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
