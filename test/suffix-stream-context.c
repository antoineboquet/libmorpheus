// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>

#include <gkstring.h>

#include "../src/gkends/nextsufftab.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	FILE *first_stream;
	char entry[MAXPATHNAME];

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(NextSuffTable(entry));
	assert(first->suffix_table_file);
	first_stream = first->suffix_table_file;

	morpheus_runtime_context_activate(second);
	assert(!second->suffix_table_file);
	assert(NextSuffTable(entry));
	assert(second->suffix_table_file != first_stream);

	morpheus_runtime_context_activate(first);
	assert(first->suffix_table_file == first_stream);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
