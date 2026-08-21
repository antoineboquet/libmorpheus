#include "greeklib_internal.h"
#include <greek.h>

#include "beta_tolower.proto.h"
/*
 * convert from beta code capital letters
 *
 * greg crane
 * may 1988
 */

/*
 * start with something like "*)/andra"
 * and end with "a)/ndra"
 */
int beta_tolower(char *word)
{
	register char * s;
	register char * t;

	if( *word != BETA_UCASE_MARKER ) return(0);

	s = word;
	while(!isalpha((unsigned char)*s)&&*s) s++;
	*word = *s;
	Xstrcpy(s,s+1);
	return(1);
}
