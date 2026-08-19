#include <gkstring.h>

#include "trimwhite.proto.h"

int trimwhite(char *s)
{
	char * starts;

	starts = s;
	while(*s) s++; s--;
	while(isspace(*s) && s > starts )
		*s-- = 0;
}
