#include <assert.h>
#include <string.h>

#include <gkstring.h>

#include "../src/greeklib/binlook.proto.h"

static int
compare_text(char *left, char *right)
{
	return(strcmp(left,right));
}

int
main(void)
{
	char entries[][8] = { "alpha", "beta", "gamma" };

	assert(binlook((char *)entries,"alpha",3,sizeof entries[0],YES,
		compare_text) == 0);
	assert(binlook((char *)entries,"beta",3,sizeof entries[0],YES,
		compare_text) == 1);
	assert(binlook((char *)entries,"gamma",3,sizeof entries[0],YES,
		compare_text) == 2);
	assert(binlook((char *)entries,"delta",3,sizeof entries[0],YES,
		compare_text) == -1);
	assert(binlook((char *)entries,"delta",3,sizeof entries[0],NO,
		compare_text) == 1);
	assert(binlook((char *)entries,"aardvark",3,sizeof entries[0],NO,
		compare_text) == 0);
	assert(binlook((char *)entries,"omega",3,sizeof entries[0],NO,
		compare_text) == 2);
	assert(binlook(NULL,"alpha",3,sizeof entries[0],YES,
		compare_text) == -1);
	assert(binlook((char *)entries,NULL,3,sizeof entries[0],YES,
		compare_text) == -1);
	assert(binlook((char *)entries,"alpha",0,sizeof entries[0],NO,
		compare_text) == -1);
	assert(binlook((char *)entries,"alpha",3,0,NO,
		compare_text) == -1);
	assert(binlook((char *)entries,"alpha",3,sizeof entries[0],YES,
		NULL) == -1);
	return(0);
}
