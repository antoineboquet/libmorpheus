#include "morphlib_internal.h"
#include <greek.h>

#include "morphstrcmp.proto.h"

static int compare_nulls(const char *s1, const char *s2, int *result)
{
	if (s1 && s2) return(0);
	if (s1 == s2) *result = 0;
	else *result = s1 ? 1 : -1;
	return(1);
}

/*
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int morphstrcmp(char *s1, char *s2)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *comptab = context->comparison_table;
	int result;

	if (compare_nulls(s1,s2,&result)) return(result);
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
	int result;

	if (compare_nulls(s1,s2,&result)) return(result);
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
	int result;

	if (!n) return(0);
	if (compare_nulls(s1,s2,&result)) return(result);
	if( ! context->comparison_table_initialized ) {
		init_comptab();
	}
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
	int result;

	if (compare_nulls(s1,s2,&result)) return(result);
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
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	unsigned char *comptab = context->comparison_table;
	int result;

	if (!n) return(0);
	if (compare_nulls(s1,s2,&result)) return(result);
	if (!context->comparison_table_initialized) init_comptab();
	while (n--) {
		while (*s1 && !isalpha((unsigned char)*s1)) s1++;
		while (*s2 && !isalpha((unsigned char)*s2)) s2++;
		result = (int)comptab[(unsigned char)*s1] -
		         (int)comptab[(unsigned char)*s2];
		if (result || !*s1) return(result);
		s1++;
		s2++;
	}
	return(0);
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
