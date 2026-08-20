#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/gkdict/derivio.proto.h"
#include "../src/morphlib/runtime_context.h"

static void
initialize_reduplication_buffers(void)
{
	char stem[] = "a";
	char keys[LONGSTRING] = { 0 };

	assert(checkaugredup(stem,keys) >= 0);
}

int
main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	char keys[LONGSTRING];

	assert(greek);
	assert(latin);
	morpheus_runtime_context_set_language(greek,GREEK);
	morpheus_runtime_context_set_language(latin,LATIN);

	previous = morpheus_runtime_context_activate(greek);
	add_deriv_cache("shared-stem","greek-keys");
	assert(stemstr_in_cache("shared-stem",keys));
	assert(!strcmp(keys,"greek-keys"));
	initialize_reduplication_buffers();

	morpheus_runtime_context_activate(latin);
	assert(!stemstr_in_cache("shared-stem",keys));
	add_deriv_cache("shared-stem","latin-keys");
	assert(stemstr_in_cache("shared-stem",keys));
	assert(!strcmp(keys,"latin-keys"));
	initialize_reduplication_buffers();

	morpheus_runtime_context_activate(greek);
	assert(stemstr_in_cache("shared-stem",keys));
	assert(!strcmp(keys,"greek-keys"));
	morpheus_runtime_context_set_language(greek,LATIN);
	assert(!stemstr_in_cache("shared-stem",keys));

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
