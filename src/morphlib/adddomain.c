#include "morphlib_internal.h"
#include <gkstring.h>
#include <limits.h>

#include "adddomain.proto.h"

int add_domain(gk_string *gstr, int n)
{
	char * p = domains_of(gstr);
	unsigned char domain;

	if (n <= 0 || n > UCHAR_MAX)
		return(-1);
	domain = (unsigned char)n;
	
	if( Xstrlen(domains_of(gstr)) >= MAXDOMAINS )
		return(-1);
		
	while(*p) {
		if((unsigned char)*p == domain)
			return(0);
		p++;
	}
	*p = (char)domain;
	return(1);
}
