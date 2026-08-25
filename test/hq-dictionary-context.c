// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>

#include <endtags.h>
#include <gkdict.h>
#include <gkstring.h>

#include "../src/gkdict/dictio.proto.h"
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
	assert(!GetHqDict());
	SetHqDict(1);
	assert(GetHqDict());

	morpheus_runtime_context_activate(second);
	assert(!GetHqDict());
	SetHqDict(0);
	assert(!GetHqDict());

	morpheus_runtime_context_activate(first);
	assert(GetHqDict());
	SetHqDict(0);
	assert(!GetHqDict());

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
