#include "greeklib_internal.h"
/*	Univ. of Calif. Greek Project	*/
/*	1985-86				*/
/*	Joshua Kosman			*/
/*	David Neel Smith		*/

#include <greek.h>
#include <string.h>


void cinsert (int c, char *p)
/* insert char c before point p */
{
	size_t length;

	if (!p) return;
	length = strlen(p);
	memmove(p+1,p,length+1);
	*p = (char)(unsigned char)c;
}
