#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/gener/gener_internal.h"
#include "../src/morphlib/morphkeys.proto.h"

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
	gk_word first = { 0 };
	gk_word second = { 0 };
	char first_entry[LONGSTRING];
	char repeated_entry[LONGSTRING];
	char second_entry[LONGSTRING];

	initialize_noun(&first,"a)nqrwp","a)/nqrwpos");
	initialize_noun(&second,"log","lo/gos");
	GenDictEntry(&first,first_entry);
	GenDictEntry(&second,second_entry);
	GenDictEntry(&first,repeated_entry);

	assert(first_entry[0]);
	assert(second_entry[0]);
	assert(strcmp(first_entry,second_entry));
	assert(!strcmp(first_entry,repeated_entry));
	return(0);
}
