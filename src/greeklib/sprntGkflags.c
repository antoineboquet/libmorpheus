#include	<gkstring.h>

#include "sprntGkflags.proto.h"

int SprintGkFlags(gk_string *, char *, size_t, const char *, int);

/*
	Created:	04.24.92
	Author:		jjake
	This is essentially the SprintGKFlags routine, excpet I have modified it to accept
	a second delimiter for the lists of Dialects,regions,domains and morph names.
	
	modified:	04.29.92
	By:			jjake
	It seems that the attributes called stem_type,GeogRegionNames and DomainNames are
	always empty. Hence I am commenting those parts out. (This was deduced from running
	morphGrinder on all of Thucydides.
	
	modified: 	04.30.92
	By:			jjake
	I got the wrong fields. It seems that derivType is the empty one, and not stemtype, and
	furthermore that there is an extra tab somewhere. So here goes.
*/
int JakeSprintGkFlags(gk_string *gstr, char *buf, size_t capacity,
                      const char *dels, const char *more_dels, int pretty)
{
	if (!more_dels) return(0);
	return(SprintGkFlags(gstr,buf,capacity,dels,pretty));
}
