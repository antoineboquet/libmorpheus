#include <gkstring.h>
#include "gkends_internal.h"
#include "endfiles.h"
#include "nextsufftab.proto.h"
#include "../morphlib/morphkeys.proto.h"
#define MAX_END_TABLE	20000
static int xstrcmp(const void *, const void *);
#define DELIMITER " "

int
indexendtables(Stemtype stype, int is_deriv)
{

	int index = 0;
	int i;
	size_t endcount = 0;
	size_t output_index;
	char **endlines;
	gk_string Gstr;
	const gk_string Blnk = { 0 };
	char * curtable, *basen, * dirp;
	char shortname[LONGSTRING+MAXPATHNAME];
	char curderivname[LONGSTRING];
	char tmp[LONGSTRING*8];
	char prevtag[LONGSTRING];
	char prevkey[LONGSTRING];
	char curtag[LONGSTRING];
	char savestr[LONGSTRING];
	char markedstr[MAXWORDSIZE];
	FILE * finput, *foutput;
	int maxstring = 0;
	
	if( is_deriv ) 
		dirp = DERIVTABLEDIR;
	else
		dirp = ENDTABLEDIR;
	
	endlines = (char **) calloc(MAX_END_TABLE,sizeof *endlines);
	
	for(;;) {
		if( is_deriv ) {
			gk_string * gstring;
			gk_word * tmpGkword;
			Derivtype derivtype;
			int rconj;
		
			curtable = NextSuffTable(tmp);
			nextkey(tmp,curderivname);
			if( ! curtable ) break;
			curtable = curderivname;
			gstring = CreatGkString(1);
			tmpGkword = CreatGkword(1);
			ScanAsciiKeys(curtable,tmpGkword,gstring,NULL);
			derivtype = derivtype_of(gstring);
			rconj = Is_regconj(gstring);
			
			FreeGkString(gstring);
			FreeGkword(tmpGkword);
			if( ! rconj ) {
				printf("[%s] not a regular conj [%o] [%o]\n", curtable, derivtype, REG_DERIV);
				continue;
			}
		} else 
			curtable=NextEndTable(&index,stype);
		if( ! curtable ) break;
		
/*
		printf("about to compile [%s]\n", curtable );
*/
		if( snprintf(shortname,sizeof shortname,"%s%cout%c%s.out",
		             dirp, DIRCHAR, DIRCHAR, curtable) >= (int)sizeof shortname ) {
			fprintf(stderr,"ending-table path is too long: %s\n",curtable);
			continue;
		}

		if(! (finput=MorphFopen(shortname,"rb"))) {
			continue;
		}
		Gstr = Blnk;	
		get_endheader(finput,&maxstring);

		while(ReadEnding(finput,&Gstr,maxstring)) {
			char * sp;
			sp = gkstring_of(&Gstr);
			Xstrcpy(savestr,sp);
			if( has_diaeresis(sp) || hasaccent(sp) || has_quant(sp) ) {
				Xstrcpy(markedstr,sp);
			} else
				markedstr[0] = 0;
if( *sp < ' ' || *sp > 126 ) printf("bad line name [%s] sp [%s]\n", shortname , sp );
			stripdiaer(sp);
			stripacc(sp);
			stripquant(sp);
			if( *sp != '*' && *(sp+strlen(sp)-1) == '*' )
				*(sp+strlen(sp)-1) = 0;
if(  ! *sp ) {
	printf("null ending in [%s]\n", curtable );
	continue;
}
			if( is_deriv ) {
				Xstrcpy(tmp,gkstring_of(&Gstr));
				strcat(tmp,"\t");
				if( strcmp(gkstring_of(&Gstr),savestr) ) {
					strcat(tmp,savestr);
				}
				SprintGkFlags(&Gstr,tmp,sizeof tmp,":",0);
			} else
				sprintf(tmp,"%s\t%s", gkstring_of(&Gstr) , NameOfStemtype(stemtype_of(&Gstr) ) );

/*
			if( is_deriv && derivtype_of(&Gstr) ) {
				strcat(tmp,":");
				strcat(tmp,NameOfDerivtype(derivtype_of(&Gstr)));
			}
*/
			if( endcount >= MAX_END_TABLE ) {
				fprintf(stderr,"more than %d endings in table! bye!\n", MAX_END_TABLE );
				break;
			}
			*(endlines+endcount) = (char *)calloc(strlen(tmp)+1,sizeof ** endlines );
			if( ! *(endlines+endcount) ) {
				fprintf(stderr,"ran out of memory at %zu endings!\n", endcount );
				return(-1);
			}
			Xstrcpy(*(endlines+endcount),tmp);
			endcount++;
		}
/*
printf("deriv [%o] name [%s]\n", derivtype_of(&Gstr), 
NameOfDerivtype(derivtype_of(&Gstr)) );
*/
		if( endcount >= MAX_END_TABLE ) break;

		fclose(finput);
	}

	qsort(endlines,endcount,sizeof * endlines, xstrcmp );
	
printf("stype [%o]\n", stype );
	if( is_deriv ) 
		basen = "derivind";
	else if( stype & PPARTMASK ) 
		basen = "vbendind";
	else
		basen = "nendind";
		
	sprintf(shortname,"%s%cindices%c%s", dirp, DIRCHAR, DIRCHAR , basen );

printf("output file:%s\n", shortname );
	if(! (foutput=MorphFopen(shortname,"w"))) {
		ErrorMess("Could not open nendind!");
		return(-1);
	}
	
	prevtag[0] = 0;
	for(output_index=0;output_index<endcount;output_index++) {

		nextkey(*(endlines+output_index),curtag);
		/*
		 * if a new keys
		 */

		if( morphstrcmp(curtag,prevtag) ) {
			if( prevtag[0] ) fprintf(foutput,"\n");
			fprintf(foutput,"%s%s%s", curtag, DELIMITER, *(endlines+output_index) );
		} else if ( strcmp(prevkey,*(endlines+output_index) ) )
			/*
			 * don't include lines such as "uiais perf_act perf_act"
			 * where the same key is repeated
			 */
			fprintf(foutput,"%s%s", DELIMITER, *(endlines+output_index) );
		Xstrcpy(prevtag,curtag);
		Xstrcpy(prevkey,*(endlines+output_index));
	}

	fprintf(foutput,"\n");
	fclose(foutput);
/*	index_list(shortname,NULL);*/
	for(output_index=0;output_index<endcount;output_index++) free(*(endlines+output_index));
	free((char *)endlines);
	return(0);

}

static int
xstrcmp(const void *left, const void *right)
{
	const char * const *p1 = left;
	const char * const *p2 = right;
	int rval;
	
	rval = morphstrcmp((char *)*p1,(char *)*p2);

/*
fprintf(stderr,"rval [%d] for [%s] and [%s]\n", rval  , *p1, *p2 );
*/
	return(rval);
}
