#include "greeklib_internal.h"
#include <greek.h>

#include "stripacute.proto.h"

/*
 * gregory crane
 *
 * harvard university
 */
 
void stripacute(char *s)
{
	stripchar(s,ACUTE);
}
