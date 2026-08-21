#include "morphlib_internal.h"
#include <gkstring.h>
#include <endfiles.h>
#include <endtags.h>
#include <stdint.h>

#include "indkeys.proto.h"

typedef struct {
	char curkey[LONGSTRING];
	char prevkey[LONGSTRING];
	int nkeys;
} index_key_state;

static void prockeyline(
	char *,
	int,
	morpheus_stemlib_offset,
	FILE *,
	index_key_state *
);

int index_list(char *listname, char *tagstring, int modulus)
{
	index_key_state state = {{0}, {0}, MODULUS + 1};
	FILE * finput;
	FILE * foutput;
	char outfile[BUFSIZ];
	char line[LONGSTRING*4];
	char curlemma[LONGSTRING];
	char field[LONGSTRING];
	morpheus_stemlib_offset curoff;
	int i;
	int taglen;
	
	if( modulus > MODULUS ) modulus = MODULUS;
	finput = MorphFopen(listname,"r");
	if( ! finput ) {
		fprintf(stderr,"Could not open input %s\n", listname );
		return(-1);
	}
	sprintf(outfile,"%s.lindex",listname);
	
	foutput = MorphFopen(outfile,"wb");
	if( ! finput ) {
		fprintf(stderr,"Could not open output  %s\n", outfile );
		return(-1);
	}
	if( tagstring ) taglen = Xstrlen(tagstring);
	for(i=0;;i++) {
		long file_offset = ftell(finput);

		if(file_offset < 0 || (uintmax_t)file_offset > UINT32_MAX) {
			fprintf(stderr, "Index offset is outside the 32-bit stemlib format\n");
			fclose(finput);
			fclose(foutput);
			return(-1);
		}
		curoff = (morpheus_stemlib_offset)file_offset;
		if( ! fgets(line,sizeof line,finput) )
			break;
		if( Xstrlen(line) >= LONGSTRING ) {
			FILE * f;
			
			if( (f=fopen("inderr","a")) ) {
				fprintf(f,"fat line %d bytes:%s\n", Xstrlen(line) , line );
				fclose(f);
			}
			printf("fat line %d bytes:%s\n", Xstrlen(line) , line );
		}
		if( is_blank(line) ) continue;
		if( line[0] == '#' ) continue;
		if( tagstring ) {
			if( ! Xstrncmp(line,tagstring,taglen) ) 
			prockeyline(line+taglen,modulus,curoff,foutput,&state);
		} else
			prockeyline(line,modulus,curoff,foutput,&state);
	}
	fclose(finput);
	fclose(foutput);
	return(0);
}


static void prockeyline(
	char *s,
	int modulus,
	morpheus_stemlib_offset curoff,
	FILE *f,
	index_key_state *state
)
{
	char curlemma[LONGSTRING];
	char * p;
	int i;
	int prntflag = 0;

	p = s;
	
	for(i=0;i<KEYLEN;i++) {
		state->curkey[i] = *p++;
		state->curkey[i+1] = 0;
		if( (! *p) || isspace( * p ) )
			break;
	}
	
	if( ++state->nkeys >= modulus && morphstrcmp(state->curkey,state->prevkey) ) {
		if( prntflag )
			fprintf(stdout,"%s\t%ld\n", state->curkey , (long)curoff );

		WriteKey(state->curkey,&curoff,f);
		state->nkeys = 0;
	} else {
		if( prntflag )
			printf("not writing key [%s]:nkeys %d modulus %d prev %s curkey [%s] curoff %ld\n",
				s,state->nkeys,modulus,state->prevkey,state->curkey,(long)curoff);
	}
	Xstrncpy(state->prevkey,state->curkey,LONGSTRING);
}
