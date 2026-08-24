#include "greeklib_internal.h"
/*	Univ. of Calif. Greek Project	*/
/*	1985-86				*/
/*	Joshua Kosman			*/
/*	David Neel Smith		*/

#include <greek.h>

#include "stripbreath.proto.h"

void stripbreath(char *word)
{
	register char *p;
	size_t length;

	if (!word) return;
	length = Xstrlen(word);
	while (length) {
		p = word + --length;
		if (Is_breath(*p))
			strsqz(p,1);
	}
}
