/*
 * gregory crane
 * 
 * harvard university
 */
 
#include <greek.h>

#include "subchar.proto.h"

void subchar(char *s, int c1, int c2)
{
	if (!s) return;
	while(*s) {
		if((unsigned char)*s == (unsigned char)c1)
			*s = (char)(unsigned char)c2;
		s++;
	}
}
