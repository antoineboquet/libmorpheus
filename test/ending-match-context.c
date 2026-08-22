#include <assert.h>

#include <gkstring.h>

#include "../src/gkends/retrends.proto.h"
#include "../src/morphlib/gkstring.proto.h"
#include "../src/morphlib/morphflags.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	gk_string *group_endings;
	char ending[MAXWORDSIZE];
	int group_ending_count;
	int i;

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
	group_endings = chckendings("oi","os_ou masc is_group",
	                            "*dios-kor","",(Dialect)0,
	                            &group_ending_count);
	assert(group_endings);
	assert(group_ending_count > 0);
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	for(i=0;i<group_ending_count;i++)
		assert(Is_group_name(morphflags_of(group_endings+i)));
	FreeGkString(group_endings);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
