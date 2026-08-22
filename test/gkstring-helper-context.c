#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/morphlib/gkstring.proto.h"
#include "../src/morphlib/adddomain.proto.h"
#include "../src/morphlib/fixacc.proto.h"
#include "../src/morphlib/morphflags.proto.h"
#include "../src/morphlib/morphkeys.proto.h"
#include "../src/morphlib/new_val.proto.h"
#include "../src/morphlib/setlang.proto.h"

static int
compare_text(char *left, char *right)
{
	return(strcmp(left,right));
}

int
main(void)
{
	gk_string entries[3] = { 0 };
	gk_string item = { 0 };
	int length = 0;

	assert(CreatGkString(0) == NULL);
	assert(add_domain(&item,1) == 1);
	assert(add_domain(&item,1) == 0);
	assert(add_domain(&item,0) == -1);
	assert(add_domain(&item,256) == -1);
	set_morphflag(morphflags_of(&item),8);
	assert(morphflags_of(&item)[0] == (MorphFlags)0200);
	add_morphflag(morphflags_of(&item),9);
	assert(morphflags_of(&item)[1] == (MorphFlags)1);
	zap_morphflag(morphflags_of(&item),8);
	assert(morphflags_of(&item)[0] == (MorphFlags)0);
	new_person(&item,PERS3);
	new_case(&item,NOMINATIVE);
	new_case(&item,ACCUSATIVE);
	new_gender(&item,MASCULINE);
	new_gender(&item,FEMININE);
	assert(person_of(forminfo_of(&item)) == PERS3);
	assert(case_of(forminfo_of(&item)) == (NOMINATIVE | ACCUSATIVE));
	assert(gender_of(forminfo_of(&item)) == (MASCULINE | FEMININE));

	set_gkstring(&item,"beta");
	add_morphflag(morphflags_of(&item),POETIC);
	ClearGkstring(&item);
	assert(!gkstring_of(&item)[0]);
	assert(no_morphflags(&item));

	set_gkstring(&item,"beta");
	length = xInsertGstr(entries,&item,length,compare_text,NO);
	set_gkstring(&item,"alpha");
	length = xInsertGstr(entries,&item,length,compare_text,NO);
	set_gkstring(&item,"gamma");
	length = xInsertGstr(entries,&item,length,compare_text,NO);

	assert(length == 3);
	assert(!strcmp(gkstring_of(entries),"alpha"));
	assert(!strcmp(gkstring_of(entries+1),"beta"));
	assert(!strcmp(gkstring_of(entries+2),"gamma"));

	{
		gk_string empty = { 0 };
		gk_string parsed = { 0 };
		gk_word parsed_word = { 0 };
		MorphFlags flags[MORPHFLAG_STORAGE_BYTES] = { 0 };
		char *oddkeys;
		char simple[MAXWORDSIZE] = "logos";
		char word[MAXWORDSIZE] = { 0 };

		set_lang(GREEK);
		putsimpleacc(simple);
		assert(simple[0]);
		FixPersAcc(&empty,flags,&empty,"",word,(word_form){ 0 },0);
		assert(!word[0]);

		assert(!ScanAsciiKeys("codex_unknown_a",&parsed_word,&parsed,NULL));
		assert(oddkeys_of(&parsed_word));
		assert(!strcmp(oddkeys_of(&parsed_word),"codex_unknown_a"));
		oddkeys = oddkeys_of(&parsed_word);
		assert(!ScanAsciiKeys("codex_unknown_b",&parsed_word,&parsed,NULL));
		assert(oddkeys_of(&parsed_word) == oddkeys);
		assert(!strcmp(oddkeys_of(&parsed_word),"codex_unknown_b"));
		free(oddkeys_of(&parsed_word));
	}
	return(0);
}
