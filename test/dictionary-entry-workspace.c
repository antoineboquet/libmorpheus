#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <gkstring.h>

#include "../src/gener/gener_internal.h"
#include "../src/morphlib/morphkeys.proto.h"
#include "../src/morphlib/runtime_context.h"

static void
initialize_noun(gk_word *word, char *stem, char *lemma)
{
	set_stem(word,stem);
	set_lemma(word,lemma);
	set_stemtype(stem_gstr_of(word),GetStemNum("os_ou"));
}

int
main(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	gk_word first = { 0 };
	gk_word invalid = { 0 };
	gk_word second = { 0 };
	char first_entry[LONGSTRING];
	char oversized_keys[LONGSTRING + 1];
	char repeated_entry[LONGSTRING];
	char second_entry[LONGSTRING];

	assert(context);
	previous = morpheus_runtime_context_activate(context);
	initialize_noun(&first,"a)nqrwp","a)/nqrwpos");
	initialize_noun(&second,"log","lo/gos");
	GenDictEntry(&first,first_entry);
	GenDictEntry(&second,second_entry);
	GenDictEntry(&first,repeated_entry);

	assert(first_entry[0]);
	assert(second_entry[0]);
	assert(strcmp(first_entry,second_entry));
	assert(!strcmp(first_entry,repeated_entry));
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	memset(oversized_keys,'x',sizeof oversized_keys - 1);
	oversized_keys[sizeof oversized_keys - 1] = 0;
	assert(!GenIrregForm(&invalid,oversized_keys,0));
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	assert(!GenIrregForm(&invalid,"codex_unknown",0));
	assert(oddkeys_of(&invalid));
	free(oddkeys_of(&invalid));
	oddkeys_of(&invalid) = NULL;
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(context);
	return(0);
}
