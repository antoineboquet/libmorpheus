#include "greeklib_internal.h"

#include "stripchar.proto.h"

void stripchar(char *s, int c)
{
	char *read;
	char *write;

	if (!s) return;
	read = s;
	write = s;
	while(*read) {
		if ((unsigned char)*read != (unsigned char)c)
			*write++ = *read;
		read++;
	}
	*write = 0;
}
