#include <gkstring.h>

#include "cmpend.proto.h"

/*
 * greg crane
 * son of morpheus
 * 2/6/87
 */
int cmpend(const char *word, const char *ending, char *stem)
{
	register const char *s1 = word;
	register const char *s2 = ending;

	if( ! *s1  || ! *s2 ) return(0);

	while(*s1) s1++; s1--;
	while(*s2) s2++; s2--;

	while(s2 >= ending) {
		if( s1 <= word ) return(0);
		if( *s1 != *s2 ) return(0);
		if( s2 == ending )
			break;
		s1--; s2--;
	}
	if( s2 == ending ) {
		Xstrcpy(stem,word);
		*(stem + (s1-word)) = 0;
		return(1);
	}
	return(0);
}
