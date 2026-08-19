#include "greeklib_internal.h"
#include <greek.h>
#include "stripzeroend.proto.h"
void stripzeroend(char *word)
{
	int n;

	n = Xstrlen(word);

	if (Is_zeroend(*(word+n-1)) )
		*(word+n-1) = 0;
}
