/*	Univ. of Calif. Greek Project	*/
/*	1985-86				*/
/*	Joshua Kosman			*/
/*	David Neel Smith		*/

#include <greek.h>
#include <string.h>

#include "strsqz.proto.h"

void strsqz(char *p, int n)
/* squeeze out n chars beginning with *p  */
{
	size_t length;
	size_t amount;

	if (!p || n <= 0) return;
	length = strlen(p);
	amount = (size_t)n;
	if (amount > length) amount = length;
	memmove(p,p+amount,length-amount+1);
}
