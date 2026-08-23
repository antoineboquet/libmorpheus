#include "greeklib_internal.h"
/*
 * grc 11/28/94
 *
 * make all long alpha-s into etas -- used to look for words that
 * are the same except for minor dialectical shift between alpha and eta
 */

#include "standalpha.proto.h"

void standalpha(char *s )
{
	char *read;
	char *write;

	if (!s) return;
	read = s;
	write = s;
	while(*read) {
		if (read[0] == 'a' && read[1] == '_') {
			*write++ = 'h';
			read += 2;
		} else {
			*write++ = *read++;
		}
	}
	*write = 0;
}
