#include <gkstring.h>
#include "gkends_internal.h"
#include "../morphlib/runtime_context_internal.h"
#include "endfiles.h" 

#include "getcurrend.proto.h"

static void
clear_ending_cache(morpheus_runtime_context *context)
{
	int i;

	for (i = 0; i < MORPHEUS_END_CACHE_SIZE; i++) {
		if (context->ending_cache[i]) {
			FreeGkString(context->ending_cache[i]);
			context->ending_cache[i] = NULL;
		}
	}
	context->ending_cache_current = 0;
	context->ending_cache_size = 0;
}

static morpheus_runtime_context *
current_ending_cache(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	int language = cur_lang();

	if (!context->ending_cache_initialized) {
		context->ending_cache_language = language;
		context->ending_cache_initialized = 1;
	} else if (context->ending_cache_language != language) {
		clear_ending_cache(context);
		context->ending_cache_language = language;
	}
	return(context);
}

gk_string *
GetCurrentEndList(gk_string *gstr, int *lnump)
{
	char fname[MAXPATHNAME];
	gk_string * CurEndList;
	FILE * f  = NULL;
	int maxend = 0;
	int rval;
	int i = 0;
	int lno = 0;
	
	CurEndList = CheckEndCache(gstr);

	if( CurEndList) {
		for(i=0;;i++) {
			if( ! gkstring_of(CurEndList+i)[0] ) break;
		}
		*lnump = i;
		return(CurEndList);
	}

	sprintf(fname,"%s/out/%s.out", ENDTABLEDIR , NameOfStemtype(stemtype_of(gstr)) );

	if( (f=MorphFopen(fname,"rb")) == NULL ) {
		fprintf(stderr,"stemtype %o, could not open %s\n", stemtype_of(gstr), fname );
		return(NULL);
	}

	lno = get_endheader(f,&maxend);
	if( lno < 0 ) {
		fprintf(stderr,"problem with endfile [%s]\n", NameOfStemtype(stemtype_of(gstr)) );
		xFclose(f);
		f = NULL;
		return(NULL);
	}

/*
printf("opening %s %d %d %ld\n", NameOfStemtype(stemtype_of(gstr)),
morpheus_runtime_context_current()->ending_cache_current, lno,
(long) (lno * (sizeof *gstr - (sizeof gkstring_of(gstr) + maxend ) )) );
*/
	
	*lnump = lno;
	CurEndList = (gk_string *) CreatGkString( lno + 2 );

	if( ! CurEndList ) {
		fprintf(stderr,"Out of memory loading in %d new endings!\n", lno+2);
		xFclose(f);
		f = NULL;
		exit(-21);
		return(NULL);
	}

	for(i=0;i<lno;i++ ) {
		rval=ReadEnding(f,CurEndList+i,maxend);
		set_stemtype(CurEndList+i,stemtype_of(gstr));

		if (rval <= 0 ) {
			fprintf(stderr,"hey! fname [%s] wanted [%d] endings got [%d]!\n", fname, lno , i );
			xFclose(f);
			f = NULL;
			return(NULL);
		}
	}
/*
fprintf(stderr,"about to close %s\n", NameOfStemtype(stemtype_of(gstr)) );
*/
	xFclose(f);
	f = NULL;

	InsertEndCache(CurEndList);
	return(CurEndList);
}

gk_string *
CheckEndCache(gk_string *gstr)
{
	morpheus_runtime_context *context = current_ending_cache();
	int i;
	
	
cacheconsistent();

	for(i=0;i<MORPHEUS_END_CACHE_SIZE;i++) {
		if(! context->ending_cache[i] ) break;
		if( stemtype_of(context->ending_cache[i]) == stemtype_of(gstr))
			return(context->ending_cache[i]);
	}
	return(NULL);
}

void
cacheconsistent(void)
{
	morpheus_runtime_context *context = current_ending_cache();
	int i;
	
	for(i=0;i<MORPHEUS_END_CACHE_SIZE;i++) {
		if( ! context->ending_cache[i] ) break;
		if( ! NameOfStemtype(stemtype_of(context->ending_cache[i])) ) {
			if( i > 1 ) 
				printf("%d) prev type %s\n", i, 
					NameOfStemtype(stemtype_of(context->ending_cache[i-1])) );
			else
				printf("first stemtype has been zapped!\n");
			break;
		}
	}
	if( i < context->ending_cache_size ) {
		printf("saw only %d of %d ending tables\n", i,
			context->ending_cache_size);
	} else
		context->ending_cache_size = i;
	
}
void
InsertEndCache(gk_string *gstr)
{
	morpheus_runtime_context *context = current_ending_cache();
	int current = context->ending_cache_current;
	
	if( context->ending_cache[current] ) {
		FreeGkString(context->ending_cache[current]);
		context->ending_cache[current] = NULL;
/*
printf("dumping %s curc %d\n",
NameOfStemtype(stemtype_of(context->ending_cache[current])), current);
*/
	}

	context->ending_cache[current] = gstr;

	if( ++context->ending_cache_current >= MORPHEUS_END_CACHE_SIZE )
		context->ending_cache_current = 0;
}
