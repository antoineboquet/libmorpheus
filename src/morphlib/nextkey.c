#include "morphlib_internal.h"
/*
 * copyright Gregory Crane
 * February 1987
 */
#include <ctype.h>
#include <string.h>

#include "nextkey.proto.h"
int nextkey(char *keylist, char *nextkey)
{
	char *a;
	char *b;

	if (!keylist || !nextkey) return(0);
	a = keylist;
	b = nextkey;
	while(isspace((unsigned char)*a)) a++;
	if( !*a ) {
		*nextkey = 0;
		return(0);
	}
	while(*a && !isspace((unsigned char)*a)) *b++ = *a++;
	*b = 0;
	while(isspace((unsigned char)*a)) a++;
	if( *a ) Xstrcpy(keylist,a);
	else  *keylist = 0;
	return(1);
}
