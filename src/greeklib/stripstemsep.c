#include "greeklib_internal.h"
/*
 * grc 2/7/88
 */
 
#include <greek.h>

#include "stripstemsep.proto.h"

void stripstemsep(char *word)
{
	register char *p;
	size_t length;

	if (!word) return;
	length = Xstrlen(word);
	while (length) {
		p = word + --length;
		if( Is_stemsep(*p) )
			strsqz(p,1);
	}
}
