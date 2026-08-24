#include "greeklib_internal.h"
/*	Univ. of Calif. Greek Project	*/
/*	1985-86				*/
/*	Joshua Kosman			*/
/*	David Neel Smith		*/

#include <greek.h>

#include "nsylls.proto.h"

int nsylls(char *word)
{
	register char *p;
	register int syllcount;
	size_t length;

	syllcount = 0;
	if (!word) return(0);
	length = Xstrlen(word);
	while (length) {
		p = word + --length;
		if (Is_vowel(*p)) {
			if (!is_diphth(p,word)) /* count first vowel only */
				syllcount++;
			}
	}
	return (syllcount);
}
