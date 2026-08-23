#include <gkstring.h>

#include "trimwhite.proto.h"

void trimwhite(char *s)
{
	char * starts;

	if (!s || !*s) return;
	starts = s;
	while(*s) s++; s--;
	while(s > starts && isspace((unsigned char)*s))
		*s-- = 0;
	if (s == starts && isspace((unsigned char)*s)) *s = 0;
}
