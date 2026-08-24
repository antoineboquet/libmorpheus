#include "greeklib_internal.h"
/*
 * gregory crane
 */
 
#include <greek.h>

#include "checkaccent.proto.h"


int checkaccent(char *word, int *syll, int *acc)
{
	if (!word || !syll || !acc) {
		if (syll) *syll = -1;
		if (acc) *acc = NOACCENT;
		return(-1);
	}
	*syll = ULTIMA;
	if( Is_accent((*acc=getaccent(word,*syll))) )
		return(ULTIMA);
		
	*syll = PENULT;
	if( Is_accent((*acc=getaccent(word,*syll))) )
		return(PENULT);
		
	*syll = ANTEPENULT;
	if( Is_accent((*acc=getaccent(word,*syll))) )
		return(ANTEPENULT);
	*acc = NOACCENT;
	*syll = -1;		
	return(-1);
}
