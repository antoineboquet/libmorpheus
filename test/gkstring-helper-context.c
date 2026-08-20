#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/morphlib/gkstring.proto.h"
#include "../src/morphlib/morphflags.proto.h"

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
	return(0);
}
