// SPDX-License-Identifier: AGPL-3.0-or-later

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <gkstring.h>

#include "../src/gkdict/compnoun.proto.h"
#include "../src/morphlib/runtime_context.h"

int
comstemtypes(char *stem, char *stem_keys, char *ending_keys)
{
	(void)stem;
	(void)stem_keys;
	(void)ending_keys;
	return(0);
}

static void
load_heads(const char *contents)
{
	FILE *stream = fmemopen((void *)contents,strlen(contents),"r");

	assert(stream);
	assert(setup_headtab_stream(stream));
	assert(!fclose(stream));
}

static void
assert_head(const char *head, const char *expected_key)
{
	char keys[BUFSIZ];
	char mutable_head[BUFSIZ];

	strcpy(mutable_head,head);
	assert(is_nomhead(mutable_head,keys));
	assert(strstr(keys,expected_key));
}

static void
assert_not_head(const char *head)
{
	char keys[BUFSIZ];
	char mutable_head[BUFSIZ];

	strcpy(mutable_head,head);
	assert(!is_nomhead(mutable_head,keys));
}

int
main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(greek);
	assert(latin);
	previous = morpheus_runtime_context_activate(greek);
	load_heads("ignored\n#logos\t:greek:\n#sw^ma\t:quantified:\n");
	assert(morpheus_runtime_context_error(greek) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	assert_head("logos","greek");
	assert_head("swma","quantified");

	morpheus_runtime_context_activate(latin);
	load_heads("#caput\t:latin:\n");
	assert_not_head("logos");
	assert_head("caput","latin");

	morpheus_runtime_context_activate(greek);
	assert_head("logos","greek");
	assert_not_head("caput");

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
