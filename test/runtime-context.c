#include <assert.h>

#include <gkstring.h>
#include <prntflags.h>

#include "../src/morphlib/morphflags.proto.h"
#include "../src/morphlib/preverb3.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/morphstrcmp.proto.h"
#include "../src/morphlib/setlang.proto.h"

int main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(greek);
	assert(latin);
	assert(cur_lang() == GREEK);

	morpheus_runtime_context_set_language(greek,GREEK);
	morpheus_runtime_context_set_language(latin,LATIN);
	assert(morpheus_runtime_context_language(greek) == GREEK);
	assert(morpheus_runtime_context_language(latin) == LATIN);

	previous = morpheus_runtime_context_activate(latin);
	assert(cur_lang() == LATIN);
	assert(morphstrcmp("a|","ai") == 0);
	assert(!is_pretty_morphflag(PERS_NAME));
	assert(is_prvb_morphflag(DISSIMILATION));
	assert(is_rawpreverb("trans"));
	set_lang(GREEK);
	assert(is_rawpreverb("upo"));
	set_lang(ITALIAN);
	assert(morpheus_runtime_context_language(latin) == ITALIAN);

	morpheus_runtime_context_activate(greek);
	assert(cur_lang() == GREEK);
	assert(morphstrcmp("a|","ai") == 0);
	assert(!is_pretty_morphflag(PERS_NAME));
	assert(is_prvb_morphflag(DISSIMILATION));
	assert(is_rawpreverb("upo"));
	morpheus_runtime_context_destroy(greek);
	assert(cur_lang() == GREEK);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
