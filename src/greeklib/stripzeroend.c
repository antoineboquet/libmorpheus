#include "greeklib_internal.h"
#include <greek.h>
#include "stripzeroend.proto.h"
void stripzeroend(char *word)
{
	size_t n;

	n = Xstrlen(word);
	if (n == 0) return;

	if (Is_zeroend(*(word+n-1)) )
		*(word+n-1) = 0;
}
