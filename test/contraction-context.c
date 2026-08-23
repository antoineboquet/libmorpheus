#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/gkends/acccompos.proto.h"
#include "../src/gkends/contract.proto.h"
#include "../src/gkends/fixeta.proto.h"
#include "../src/morphlib/gkstring.proto.h"
#include "../src/morphlib/loadeuph.proto.h"
#include "../src/morphlib/runtime_context.h"

static void
assert_euphony(char *input, char *expected)
{
	gk_string source = { 0 };
	gk_string *results;

	strcpy(gkstring_of(&source),input);
	results = do_euph(&source,(Dialect)0);
	assert(results);
	assert(!strcmp(gkstring_of(results),expected));
	FreeGkString(results);
}

static void
assert_contraction(char *input, char *expected)
{
	gk_string source = { 0 };
	gk_string *results;

	strcpy(gkstring_of(&source),input);
	results = poss_contracts(&source,(Dialect)0);
	if (!expected) {
		assert(!results);
		return;
	}
	assert(results);
	assert(!strcmp(gkstring_of(results),expected));
	FreeGkString(results);
}

int
main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(greek);
	assert(latin);
	morpheus_runtime_context_set_language(greek,GREEK);
	morpheus_runtime_context_set_language(latin,LATIN);

	previous = morpheus_runtime_context_activate(greek);
	{
		int count = 1;

		assert(!load_euph_tab(NULL,&count,NO));
		assert(count == 0);
		assert(morpheus_runtime_context_error(greek) ==
		       MORPHEUS_RUNTIME_ERROR_INTERNAL);
		morpheus_runtime_context_clear_error(greek);
		assert(!load_euph_tab("table",NULL,NO));
		assert(morpheus_runtime_context_error(greek) ==
		       MORPHEUS_RUNTIME_ERROR_INTERNAL);
		morpheus_runtime_context_clear_error(greek);
		assert(count_rlines(NULL) == -1);
		assert(morpheus_runtime_context_error(greek) ==
		       MORPHEUS_RUNTIME_ERROR_INTERNAL);
		morpheus_runtime_context_clear_error(greek);
	}
	assert_euphony("zs","s");
	assert_contraction("ea","h");
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	assert(!fix_eta(NULL));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	{
		gk_string source = { 0 };
		char longword[MAXWORDSIZE];
		size_t i;

		longword[0] = 'r';
		longword[1] = 'h';
		for (i = 2; i < sizeof longword - 1; i++) longword[i] = 'a';
		longword[sizeof longword - 1] = 0;
		set_gkstring(&source,longword);
		set_stemtype(&source,(Stemtype)DECL1);
		assert(!fix_eta(&source));
		assert(morpheus_runtime_context_error(greek) ==
		       MORPHEUS_RUNTIME_ERROR_INTERNAL);
		morpheus_runtime_context_clear_error(greek);
	}
	AccComposForm(NULL);
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);

	morpheus_runtime_context_activate(latin);
	assert_euphony("ents","e_ns");
	assert_contraction("ea",NULL);
	assert(morpheus_runtime_context_error(latin) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);

	morpheus_runtime_context_activate(greek);
	assert_euphony("zs","s");
	assert_contraction("ea","h");
	morpheus_runtime_context_set_language(greek,LATIN);
	assert_euphony("ents","e_ns");
	assert_contraction("ea",NULL);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
