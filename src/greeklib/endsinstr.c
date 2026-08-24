#include <greek.h>
#include <string.h>

#include "endsinstr.proto.h"

static int ignored_diacritic(char c)
{
	return(Is_accent(c) || Is_diaeresis(c));
}

/*
 * s1 ends with s2?
 */
int ends_in(const char *s1, const char *s2)
{
	const char *p1;
	const char *p2;

	if (!s1 || !s2) return(0);
	p1 = s1 + strlen(s1);
	p2 = s2 + strlen(s2);
	for (;;) {
		while (p2 > s2 && ignored_diacritic(p2[-1])) p2--;
		if (p2 == s2) return(1);
		while (p1 > s1 && ignored_diacritic(p1[-1])) p1--;
		if (p1 == s1) return(0);
		if (*--p1 != *--p2) return(0);
	}
}
