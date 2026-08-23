#include "greeklib_internal.h"
#include <string.h>
#include "xstrings.proto.h"

int
Xstrcpy(char *s1, const char *s2)
{
	if (!s1 || !s2) return(0);
	memmove(s1,s2,strlen(s2)+1);
	return(1);
}

int
Xstrncpy(char *s1, const char *s2, size_t len)
{
	size_t source_length;
	size_t copy_length;

	if (!s1 || !s2 || !len) return(0);
	source_length = strlen(s2);
	copy_length = source_length < len ? source_length : len - 1;
	memmove(s1,s2,copy_length);
	s1[copy_length] = 0;
	return(source_length < len);
}


void Xstrncat(char *s1, const char *s2, size_t len)
{
	size_t dest_length = 0;
	size_t source_length;
	size_t copy_length;

	if (!s1 || !s2 || !len) return;
	while (dest_length < len && s1[dest_length]) dest_length++;
	if (dest_length == len) {
		s1[len-1] = 0;
		return;
	}
	source_length = strlen(s2);
	copy_length = source_length < len - dest_length ?
	              source_length : len - dest_length - 1;
	memmove(s1+dest_length,s2,copy_length);
	s1[dest_length+copy_length] = 0;
}

int Xstrncmp(const char *s1, const char *s2, size_t len)
{
	return(strncmp(s1,s2,(size_t)len));
}

size_t Xstrlen(const char *s)
{
	return(strlen(s));
}

/*
Xstrcpy(char *s1, const char *s2)
{
	while(*s2) {
		*s1++ = *s2++;
	}
	*s1 = 0;
}
*/
