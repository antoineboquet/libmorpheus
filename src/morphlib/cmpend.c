#include <gkstring.h>

#include "cmpend.proto.h"

/*
 * greg crane
 * son of morpheus
 * 2/6/87
 */
int cmpend(const char *word, const char *ending, char *stem)
{
	size_t word_length;
	size_t ending_length;

	if (!word || !ending || !stem) return(0);
	word_length = strlen(word);
	ending_length = strlen(ending);
	if (!ending_length || ending_length >= word_length) return(0);
	if (strcmp(word+word_length-ending_length,ending)) return(0);
	Xstrcpy(stem,word);
	stem[word_length-ending_length] = 0;
	return(1);
}
