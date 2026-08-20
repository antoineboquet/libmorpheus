#include <assert.h>

#include <endtags.h>
#include <gkdict.h>
#include <gkstring.h>

#include "../src/gkdict/dictio.proto.h"
#include "../src/morphlib/morphpath.proto.h"
#include "../src/morphlib/runtime_context.h"

static void
assert_indeclinable(char *word, int expected_file_opens)
{
	char keys[LONGSTRING];
	int before = NumFilesOpened();

	assert(chckindecl(word,keys));
	assert(keys[0]);
	assert(NumFilesOpened() == before + expected_file_opens);
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
	assert_indeclinable("kai",2);
	assert_indeclinable("kai",1);

	morpheus_runtime_context_activate(latin);
	assert_indeclinable("et",2);
	assert_indeclinable("et",1);

	morpheus_runtime_context_activate(greek);
	assert_indeclinable("kai",1);
	morpheus_runtime_context_set_language(greek,LATIN);
	assert_indeclinable("et",2);

	morpheus_runtime_context_activate(latin);
	assert_indeclinable("et",1);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
