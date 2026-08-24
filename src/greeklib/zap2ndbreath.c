#include "greeklib_internal.h"
#include <greek.h>
#include <string.h>

#include "zap2ndbreath.proto.h"

/*
 * "e)nh(/bwsa" --> "e)nh/bwsa"
 * "parh(/bwsa" --> "parh/bwsa"
 */

void zap_extra_breath(char *s)
{
	int breath_is_extra = 0;
	
	if (!s) return;
	if( *s == 'r' && *(s+1) == ROUGHBR ) s += 2;
	
	while(*s) {
	
		if( Is_cons(*s) ) {
			breath_is_extra = 1;
			s++;
			continue;
		}
		
		if( *s == ROUGHBR || *s == SMOOTHBR  ) {
			if( ! breath_is_extra ) {
				breath_is_extra = 1;
				s++;
				continue;
			}
			strsqz(s,1);
			continue;
		}
		s++;
	}
}

int has_extra_breath(char *s)
{	
	int breath_is_extra = 0;

	if (!s) return(0);
	if( *s == 'r' && *(s+1) == ROUGHBR ) s += 2;
	while(*s) {
		if( Is_cons(*s) ) {
			breath_is_extra = 1;
			s++;
			continue;
		}
	
		if( *s == ROUGHBR || *s == SMOOTHBR) {
			if( ! breath_is_extra ) {
				breath_is_extra = 1;
				s++;
				continue;
			}
			return(1);
		}
		s++;
	}
	return(0);
}

/*
 * grc 11/17/94
 *
 * e.g., go from a)nanti/r)r(hton to a)nanti/rrhton
 */
void zap_rr_breath(char *s)
{
	if (!s || !*s) return;
	s++; /* careful about r(i/ptw etc. */
	while(*s) {
		if( *s=='r' && *(s+1) == ')' 
		&&  *(s+2)=='r' && *(s+3) == '(' ) {
			s[1] = 'r';
			memmove(s+2,s+4,strlen(s+4)+1);
		}
		s++;
	}
}
