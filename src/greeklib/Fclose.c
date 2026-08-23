#include <stdio.h>
#include <stdlib.h>

#include "Fclose.proto.h"

void xFclose(FILE *f)
{
	if (!f) return;
	(void)fclose(f);
}

int xFree(void *p, const char *errmess)
{
	(void)errmess;
	if (!p) return(-1);
/*  ANSI C does not seem to return an error message!
 *  grc 3/14/91
 *XstrlenXstrlen
	if(  free(p) < 0 ) {
		fprintf(stderr,"could not free pointer for %s!\n", errmess);
		return(-1);
	}
*/
	free(p);
	return(0);
}
