#include "greeklib_internal.h"
#include <greek.h>

#include "normucase.proto.h"
/*
 * convert from beta code capital letters
 *
 * greg crane
 * february 1987
 */

/*
 * start with something like "*(/ellhn"
 * and end with "E(/llhn"
 */
int normucase(char *word)
{
	register char * s;

	if (!word) return(0);
	if( *word != BETA_UCASE_MARKER ) return(0);

	s = word;
	while(!isalpha((unsigned char)*s)&&*s) s++;

	/*
	 * in case of "*(/ellhn", s points now to "ellhn"
 	 */
	if( ! islower((unsigned char)*s) )
		return(0);
	*word = (char)toupper((unsigned char)*s);
	/*
 	 * word now "E(/ellhn"
	 */
	strsqz(s,1);
	/*
	 * word now "E(/llhn"
	 */
	return(1);
}
