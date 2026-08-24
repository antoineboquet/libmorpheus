#include "greeklib_internal.h"
/*
 *	greg crane
 *	
 *	february 1987
 */

#include <greek.h>

#include "stripdiaer.proto.h"

void stripdiaer(char *word)
{
	register char *p;
	size_t length;

	if (!word) return;
	length = Xstrlen(word);
	while (length) {
		p = word + --length;
		if (*p == DIAERESIS )
			strsqz(p,1);
	}
}
