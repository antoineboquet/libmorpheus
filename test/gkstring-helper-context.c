#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/morphlib/gkstring.proto.h"
#include "../src/morphlib/adddomain.proto.h"
#include "../src/morphlib/fixacc.proto.h"
#include "../src/morphlib/morphflags.proto.h"
#include "../src/morphlib/morphkeys.proto.h"
#include "../src/morphlib/new_val.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/setlang.proto.h"
#include "../src/greeklib/getsyll.proto.h"
#include "../src/greeklib/nsylls.proto.h"
#include "../src/greeklib/stripacc.proto.h"
#include "../src/greeklib/stripbreath.proto.h"
#include "../src/greeklib/stripdiaer.proto.h"
#include "../src/greeklib/stripquant.proto.h"
#include "../src/greeklib/stripstemsep.proto.h"
#include "../src/greeklib/xstrings.proto.h"

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
	gk_word word_item = { 0 };
	char empty[] = "";
	morpheus_runtime_context *context = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	int length = 0;

	assert(context);
	previous = morpheus_runtime_context_activate(context);
	assert(CreatGkString(0) == NULL);
	assert(CreatGkAnal(0) == NULL);
	assert(CreatGkword(0) == NULL);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	assert(CreatGkString(-1) == NULL);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	assert(CreatGkAnal(-1) == NULL);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	assert(CreatGkword(-1) == NULL);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	ClearGkstring(NULL);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	CpGkAnal(NULL,&word_item);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	CpGkAnal(&word_item,NULL);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	assert(!stripacc(empty));
	stripbreath(empty);
	stripdiaer(empty);
	stripquant(empty);
	stripstemsep(empty);
	assert(!nsylls(empty));
	assert(getsyll(empty,ULTIMA) == P_ERR);
	assert(getsyll2(empty,ULTIMA) == empty);
	assert(!stripacc(NULL));
	stripbreath(NULL);
	stripdiaer(NULL);
	stripquant(NULL);
	stripstemsep(NULL);
	assert(!nsylls(NULL));
	assert(getsyll(NULL,ULTIMA) == P_ERR);
	assert(getsyll2(NULL,ULTIMA) == P_ERR);
	{
		struct {
			char value[5];
			char guard[4];
		} bounded = { "", "end" };
		char overlap_left[8] = "abcdef";
		char overlap_right[8] = "abc";
		char one[1] = { 'x' };
		char unchanged[5] = "keep";

		assert(!Xstrncpy(bounded.value,"abcdef",sizeof bounded.value));
		assert(!strcmp(bounded.value,"abcd"));
		assert(!strcmp(bounded.guard,"end"));
		assert(Xstrncpy(bounded.value,"abc",sizeof bounded.value));
		assert(!strcmp(bounded.value,"abc"));
		assert(Xstrncpy(overlap_left,overlap_left+2,sizeof overlap_left));
		assert(!strcmp(overlap_left,"cdef"));
		assert(Xstrncpy(overlap_right+1,overlap_right,
		                sizeof overlap_right - 1));
		assert(!strcmp(overlap_right,"aabc"));
		assert(!Xstrncpy(unchanged,"drop",0));
		assert(!strcmp(unchanged,"keep"));
		assert(!Xstrncpy(one,"value",sizeof one));
		assert(!one[0]);
		assert(!Xstrncpy(NULL,"value",5));
		assert(!Xstrncpy(unchanged,NULL,sizeof unchanged));
	}
	{
		struct {
			char value[6];
			char guard[4];
		} bounded = { "abc", "end" };
		char overlap[8] = "abc";
		char one[1] = { 'x' };
		char unterminated[4] = { 'a', 'b', 'c', 'd' };
		char unchanged[5] = "keep";

		Xstrncat(bounded.value,"def",sizeof bounded.value);
		assert(!strcmp(bounded.value,"abcde"));
		assert(!strcmp(bounded.guard,"end"));
		Xstrncat(overlap,overlap,sizeof overlap);
		assert(!strcmp(overlap,"abcabc"));
		Xstrncat(unterminated,"x",sizeof unterminated);
		assert(!strcmp(unterminated,"abc"));
		Xstrncat(unchanged,"drop",0);
		assert(!strcmp(unchanged,"keep"));
		Xstrncat(one,"value",sizeof one);
		assert(!one[0]);
		Xstrncat(NULL,"value",5);
		Xstrncat(unchanged,NULL,sizeof unchanged);
	}
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
	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(context);
	return(0);
}
