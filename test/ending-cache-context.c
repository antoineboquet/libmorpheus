#include <assert.h>

#include <gkstring.h>

#include "../src/gkends/getcurrend.proto.h"
#include "../src/morphlib/morphkeys.proto.h"
#include "../src/morphlib/morphpath.proto.h"
#include "../src/morphlib/runtime_context.h"

static gk_string *
load_endings(char *stem_name, int expected_file_opens)
{
	gk_string request = { 0 };
	gk_string *endings;
	int before;
	int count;

	set_stemtype(&request,GetStemNum(stem_name));
	assert(stemtype_of(&request));
	before = NumFilesOpened();
	endings = GetCurrentEndList(&request,&count);
	assert(endings);
	assert(count > 0);
	assert(NumFilesOpened() == before + expected_file_opens);
	return(endings);
}

int
main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	gk_string *greek_endings;
	gk_string *latin_endings;

	assert(greek);
	assert(latin);
	morpheus_runtime_context_set_language(greek,GREEK);
	morpheus_runtime_context_set_language(latin,LATIN);

	previous = morpheus_runtime_context_activate(greek);
	greek_endings = load_endings("os_ou",1);
	assert(load_endings("os_ou",0) == greek_endings);

	morpheus_runtime_context_activate(latin);
	latin_endings = load_endings("us_us",1);
	assert(load_endings("us_us",0) == latin_endings);

	morpheus_runtime_context_activate(greek);
	assert(load_endings("os_ou",0) == greek_endings);
	morpheus_runtime_context_set_language(greek,LATIN);
	assert(load_endings("us_us",1) != latin_endings);

	morpheus_runtime_context_activate(latin);
	assert(load_endings("us_us",0) == latin_endings);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
