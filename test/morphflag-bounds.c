#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/morphlib/morphflags.proto.h"

int main(void)
{
	gk_string source = {0};
	gk_string result = {0};
	MorphFlags snapshot[MORPHFLAG_STORAGE_BYTES];

	add_morphflag(morphflags_of(&source),GROUP_NAME);
	assert(Is_group_name(morphflags_of(&source)));
	assert(domains_of(&source)[0] == 0);
	assert(domains_of(&source)[1] == 0);

	set_morphflags(&result,morphflags_of(&source));
	assert(Is_group_name(morphflags_of(&result)));
	memcpy(snapshot,morphflags_of(&result),sizeof snapshot);
	add_morphflag(morphflags_of(&result),0);
	add_morphflag(morphflags_of(&result),-1);
	add_morphflag(morphflags_of(&result),MORPHFLAG_MAX+1);
	assert(!memcmp(snapshot,morphflags_of(&result),sizeof snapshot));
	assert(!has_morphflag(morphflags_of(&result),0));
	assert(!has_morphflag(morphflags_of(&result),MORPHFLAG_MAX+1));

	zap_morphflag(morphflags_of(&result),GROUP_NAME);
	assert(!Is_group_name(morphflags_of(&result)));
	assert(no_morphflag(morphflags_of(&result)));
	return 0;
}
