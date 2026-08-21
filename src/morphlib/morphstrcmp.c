#include "morphlib_internal.h"
#include <greek.h>

#include "morphstrcmp.proto.h"

/*
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int morphstrcmp(char *s1, char *s2)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *comptab = context->comparison_table;

	if( ! context->comparison_table_initialized ) {
		init_comptab();
	}
/*
fprintf(stderr,"looking at [%s] and [%s]\n", s1, s2 );
*/
	while(1) {
		if(comptab[(unsigned char)*s1] != comptab[(unsigned char)*s2])
			return((int)(comptab[(unsigned char)*s1] -
				comptab[(unsigned char)*s2]));
		if (*s1=='\0')
			return(0);
		s1++; s2++;
	}
	return(0);
}

int betastrcmp(char *s1, char *s2)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *betatab = context->beta_table;

	if( ! context->beta_table_initialized ) {
		init_betatab();
	}
	while(1) {
		while(*s1&&!betatab[(unsigned char)*s1]) s1++;
		while(*s2&&!betatab[(unsigned char)*s2]) s2++;

		if(betatab[(unsigned char)*s1] != betatab[(unsigned char)*s2])
			return((int)(betatab[(unsigned char)*s1] -
				betatab[(unsigned char)*s2]));
		if (*s1=='\0')
			return(strcmp(s1,s2));
		s1++; s2++;
	}
/*
 * if the two strings are equal alphanumerically,
 * then check the rest
 */
 
	return(strcmp(s1,s2));

}

int morphstrncmp(const char *s1, const char *s2, size_t n)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *comptab = context->comparison_table;

	if( ! context->comparison_table_initialized ) {
		init_comptab();
	}
	if (n <= 0) return ( 0 );
	for (; --n && (comptab[(unsigned char)*s1] ==
		comptab[(unsigned char)*s2]); s1++, s2++) {
		if (!*s1) break;
	}
		
	return ((int)(comptab[(unsigned char)*s1] -
		comptab[(unsigned char)*s2]));
}

int dictstrcmp(char *s1, char *s2)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *comptab = context->comparison_table;
	register char * t1, * t2;
	t1 = s1; t2 = s2;
	if( ! context->comparison_table_initialized ) {
		init_comptab();
	}

	while(1) {
		while(! isalpha((unsigned char)*s1) && *s1 != '|'  && *s1 != HARDLONG && *s1 ) s1++;
		while(! isalpha((unsigned char)*s2) && *s2 != '|'  && *s2 != HARDLONG && *s2 ) s2++;
		if(comptab[(unsigned char)*s1] != comptab[(unsigned char)*s2])
			return(comptab[(unsigned char)*s1] -
				comptab[(unsigned char)*s2]);
		if (*s1=='\0'||isspace((unsigned char)*s1))

			return(0);
 /*
 			return(Xstrlen(t1)-Xstrlen(t2));
 */
		s1++; s2++;
/*
		while(! isalpha((unsigned char)*s1) && *s1 != '|'  && *s1 ) s1++;
		while(! isalpha((unsigned char)*s2) && *s2 != '|' && *s2 ) s2++;
*/
	}
}

int dictstrncmp(const char *s1, const char *s2, size_t n)
{
	char b1[BUFSIZ/2];
	char b2[BUFSIZ/2];
	register char * s;
	
	s = b1;
	while(*s1) {
		if( ! isalpha((unsigned char)*s1) ) {
			s1++;
			continue;
		}
		*s++ = *s1++;
	}
	*s = 0;
	
	s = b2;
	while(*s1) {
		if( ! isalpha((unsigned char)*s2) ) {
			s2++;
			continue;
		}
		*s++ = *s2++;
	}
	*s = 0;
	
	return(morphstrncmp(b1,b2,n));
}

void init_comptab(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *comptab = context->comparison_table;
	int i;
	
	context->comparison_table_initialized = 1;
	for(i=0;i<MORPHEUS_BYTE_TABLE_SIZE;i++)
		comptab[i] = (unsigned char)i;
	comptab['|'] = 'i'; /* iota subscript matches as an 'i' */
}

void init_betatab(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *betatab = context->beta_table;
	int i;
	
	context->beta_table_initialized = 1;
	for(i=0;i<MORPHEUS_BYTE_TABLE_SIZE;i++) {
/*		if( isalpha(i) )*/
			betatab[i] = (unsigned char)i;
	}
	betatab['|'] = 'i'; /* iota subscript matches as an 'i' */
	set_gkorder(betatab);
}
