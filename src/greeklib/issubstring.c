/*
 * Gregory Crane
 * Harvard University
 * February 1987
 *
 * 8/31/88
 */
 
 /*
  * find s1 in s2 
  */
#include <string.h>

#include "issubstring.proto.h"
char * 
is_substring(char *s1, char *s2)
{
	if (!s1 || !s2 || !*s1) return(NULL);
	return(strstr(s2,s1));
}
