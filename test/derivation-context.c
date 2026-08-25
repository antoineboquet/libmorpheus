// SPDX-License-Identifier: AGPL-3.0-or-later

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
	char keys[LONGSTRING] = "not empty";
	char oversized_keys[LONGSTRING + 1];
	char oversized_stem[MAXWORDSIZE + 1];

	assert(greek);
	assert(latin);
	morpheus_runtime_context_set_language(greek,GREEK);
	morpheus_runtime_context_set_language(latin,LATIN);

	previous = morpheus_runtime_context_activate(greek);
	assert(!stemstr_in_cache("missing",keys));
	assert(!keys[0]);
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
	assert(!keys[0]);

	assert(!ends_in_vowel("logos"));
	assert(ends_in_vowel("logo"));
	assert(ends_in_vowel("logo/"));
	assert(!ends_in_vowel(""));
	assert(!ends_in_vowel("///"));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	assert(!ends_in_vowel(NULL));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	strcpy(keys,"not empty");
	assert(!stemstr_in_cache(NULL,keys));
	assert(!keys[0]);
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	assert(!stemstr_in_cache("shared-stem",NULL));
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	add_deriv_cache(NULL,"keys");
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	add_deriv_cache("stem",NULL);
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	memset(oversized_stem,'s',sizeof oversized_stem - 1);
	oversized_stem[sizeof oversized_stem - 1] = 0;
	add_deriv_cache(oversized_stem,"keys");
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);
	memset(oversized_keys,'k',sizeof oversized_keys - 1);
	oversized_keys[sizeof oversized_keys - 1] = 0;
	add_deriv_cache("stem",oversized_keys);
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(greek);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
