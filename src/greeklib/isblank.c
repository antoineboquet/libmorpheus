#include <ctype.h>

#include "isblank.proto.h"

int is_blank(char *s)
{
	while(*s && isspace((unsigned char)*s)) s++;
	if( ! *s  ) return(1);
	return(0);
}
