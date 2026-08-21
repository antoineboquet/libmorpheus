#include <gkstring.h>
#include "gkends_internal.h"
#include "../morphlib/runtime_context_internal.h"

#include "../morphlib/morphstrcmp.proto.h"
#include "endfiles.h"
#include "gkdict.h"
#define MAXENDTAGS 100

#include "endindex.proto.h"

/*int dictstrcmp(), dictstrncmp(), morphstrcmp(), morphstrncmp();*/

static endind *load_end_index(endind **, char *, const char *);

static endind *
load_end_index(endind **slot, char *filename, const char *description)
{
	if (*slot) return(*slot);
	*slot = calloc(1,sizeof **slot);
	if (!*slot) {
		fprintf(stderr,"could not allocate %s\n",description);
		return(NULL);
	}
	if (!init_endind(filename,*slot)) {
		free(endbuffer_of(*slot));
		free(endeptr_of(*slot));
		free(*slot);
		*slot = NULL;
	}
	return(*slot);
}

int
chcknend(char *endstr, char *keys)
{
	long startoff;
	char tmpendstr[MAXWORDSIZE+1];
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	endind *NomEtags;

	Xstrncpy(tmpendstr,endstr,(int)sizeof  tmpendstr);
	/*
	 * ignore '_' (long marker), so that a simple 'a' will match
	 * an "a_".
	 */
	stripquant(tmpendstr);
	
	NomEtags = load_end_index(&context->nominal_ending_index,NENDLIST,
		"nominal ending index");
	if (!NomEtags) return(0);
	return( checkendind(NomEtags,tmpendstr,keys,morphstrncmp));
}


int
chckdictent(char * possent, char *keys)
{
	long startoff;
	char tmpendstr[MAXWORDSIZE+1];
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	endind *DictEntTags;

	Xstrncpy(tmpendstr,possent,(int)sizeof  tmpendstr);
	/*
	 * ignore '_' (long marker), so that a simple 'a' will match
	 * an "a_".
	 */
	stripquant(tmpendstr);
	
	DictEntTags = load_end_index(&context->dictionary_entry_index,DICTENTLIST,
		"dictionary entry index");
	if (!DictEntTags) return(0);
	return( checkendind(DictEntTags,tmpendstr,keys,strncmp));
}

int
chckcmpvb(char *endstr, char *keys)
{
	long startoff;
	char tmpendstr[MAXWORDSIZE+1];
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	endind *CmpVbtags;

	Xstrncpy(tmpendstr,endstr,(int)sizeof  tmpendstr);
	/*
	 * ignore '_' (long marker), so that a simple 'a' will match
	 * an "a_".
	 */
	stripquant(tmpendstr);
	
	CmpVbtags = load_end_index(&context->compound_verb_index,CMPVBLIST,
		"compound verb index");
	if (!CmpVbtags) return(0);
	return( checkendind(CmpVbtags,tmpendstr,keys,strncmp));
}

int
chckend(char *endstring) 
{
	char tmp[LONGSTRING];

	tmp[0] = 0;

	return(chckvend(endstring,tmp)||chcknend(endstring,tmp));
}

int
chckvend(char *endstr, char *keys)
{
	long startoff;
	int curhit;
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	endind *VbEtags;
	
	VbEtags = load_end_index(&context->verb_ending_index,VENDLIST,
		"verb ending index");
	if (!VbEtags) return(0);
	curhit = checkendind(VbEtags,endstr,keys,morphstrncmp);

	return(curhit);
}

int
chckvstem(char *stemstr, char *keys)
{
	long startoff;
	int curhit;
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	endind *VstemEtags;
	
	VstemEtags = load_end_index(&context->verb_stem_index,VBINDEX,
		"verb stem index");
	if (!VstemEtags) return(0);
	curhit = checkendind(VstemEtags,stemstr,keys,morphstrncmp);

	return(curhit);
}

int
chckdvend(char *endstr, char *keys)
{
	long startoff;
	int curhit;
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	endind *DerEtags;
	
	DerEtags = load_end_index(&context->derivation_ending_index,DERENDLIST,
		"derivation ending index");
	if (!DerEtags) return(0);

	curhit = checkendind(DerEtags,endstr,keys,morphstrncmp);

	return(curhit);
}

endind *
init_endind(char *fname, endind *etags)
{
	FILE * f;
	register char * s;
	register char * t;
	char ** pp;
	long flen;
	int nread;
	long i;
	int j;
	int nlines = 0;
	long sofar = 0;

/*
 * grc 3/12/91
 * clalloc() seems not to exist in ANSI C
 	char * clalloc();
 */
 	if( (f=MorphFopen(fname,"r"))==NULL) {
		fprintf(stderr,"init_endind: could not open %s\n", fname );
		return( NULL );
	}
	fseek(f,0L,2);
	flen = ftell(f);
	fseek(f,0L,0);

/*
 * The ANSI port replaced the historical clalloc allocation of flen + 1
 * elements with calloc.
 */
	if( !(endbuffer_of(etags) = (char *)calloc((size_t)flen + 1, (size_t)sizeof * endbuffer_of(etags)  ))) {

		fprintf(stderr,"could not build buffer for endtags\n");
		xFclose(f);
		return(NULL);
	}
	s = endbuffer_of(etags);
	for(;;) {		
/*
	nread = vax_fread(s,(int)sizeof  * s, (int)flen, f);
*/
		nread = vax_fread((char *)s+sofar,sizeof  * s, BUFSIZ, f);
		if( nread <= 0 ) break;
		sofar += (long)nread;
	}
	xFclose(f);
	for(i=0;i<sofar;i++) {
		if(*(s+i) == '\n' )
			nlines++;
	}
	nlines++;

  	endeptr_of(etags) = (char **) calloc((size_t)nlines,(size_t)sizeof * endeptr_of(etags));
	if( ! endeptr_of(etags) ) {
		fprintf(stderr,"ran out of memory in init_endind\n");
		return(NULL);
	}
	pp = endeptr_of(etags);
	for(i=0;i<nlines;i++) {
		*(pp+i) = s;
		while(*s && *s != '\n')
			s++;
		if( ! *s ) 
			break;
		else if( *s == '\n' ) {
			*s = 0;
			s++;
		}
	}

	endlen_of(etags) = nlines;
/*
	printf("nread %d flen %ld nlines %d\n", nread ,flen , nlines);
	for(i=0;i<10;i++)
		printf("%d) [%s]\n", i , *(pp+i) );
	getchar();
*/
	return(etags);
}

int
checkendind(endind *etags, char *endstr, char *keys, int (*scmp)(const char *, const char *, size_t))
{
	int high = 0;
	int low = 0;
	int mid = 0;
	int comp;
	int ntags;
	char curtag[MAXWORDSIZE];
	size_t taglen;
	char ** pp;
	
	/*
	 * the table is of the form "ending<SPACE>key1<SPACE>key2<SPACE> ..."
	 */
	Xstrncpy(curtag,endstr,MAXWORDSIZE);
	Xstrncat(curtag," ",MAXWORDSIZE);
	taglen = strlen(curtag);
	
	ntags = endlen_of(etags);
	pp = endeptr_of(etags);

	high = ntags-1;
	low = 0 ;
	
	while( low<=high ) {
		mid = (low+high)/2 ;
		/*comp = morphstrncmp( curtag , *(pp+mid) , taglen );*/
		comp = (*scmp)( curtag , *(pp+mid) , taglen );
/*
fprintf(stderr,"comparing [%s] and [%s]\n", curtag , *(pp+mid) );
*/
		if( comp < 0 ) 
			high = mid - 1 ;
		else if ( comp > 0 )
			low = mid + 1;
		else  { /* found match */
/*
printf("A returning with curtag [%s] tagstring [%s]\n", curtag, *(pp+mid) );
*/
			Xstrncpy(keys,*(pp+mid) + taglen,LONGSTRING);
			return(1);
		}
	}
/*	if( mid > 0 ) {
		for(i=mid-1;i<ntags;i++) {
			if( morphstrcmp(curtag,tagstring_of(etags+i)) < 0 ) break;
		}
		if( i > 0 ) i--;
	} else
		i = 0;
		*/
/*
printf("B returning with curtag [%s] tagstring [%s] and off %d\n", curtag, tagstring_of(etags+i) , tagoffset_of(etags+i) );
*/
	*keys = 0;
	return(0);
}
