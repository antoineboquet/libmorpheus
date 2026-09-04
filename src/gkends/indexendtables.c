/* SPDX-License-Identifier: MPL-2.0 */

#include <gkstring.h>
#include <ctype.h>
#include "gkends_internal.h"
#include "../morphlib/runtime_context_internal.h"
#include "endfiles.h"
#include "indexendtables.proto.h"
#include "nextsufftab.proto.h"
#include "../morphlib/morphkeys.proto.h"
#include "../morphlib/morphpath.proto.h"
#define MAX_END_TABLE	20000
static int xstrcmp(const void *, const void *);
static void free_endlines(char **, size_t);
static int read_table_name(FILE *, char *, size_t, size_t *);
#define DELIMITER " "

int
indexendtables(Stemtype stype, int is_deriv)
{
	return indexendtables_from_list(stype,is_deriv,NULL);
}

int
indexendtables_from_list(Stemtype stype, int is_deriv,
                         const char *table_list_path)
{
	int index = 0;
	int i;
	int read_status;
	size_t endcount = 0;
	size_t output_index;
	char **endlines;
	gk_string Gstr;
	const gk_string Blnk = { 0 };
	char * curtable, *basen, * dirp;
	char shortname[LONGSTRING+MAXPATHNAME];
	char temporary_shortname[LONGSTRING+MAXPATHNAME];
	char output_path[BUFSIZ];
	char temporary_path[BUFSIZ];
	char curderivname[LONGSTRING];
	char listed_table[LONGSTRING];
	char tmp[LONGSTRING*8];
	char prevtag[LONGSTRING];
	char prevkey[LONGSTRING];
	char curtag[LONGSTRING];
	char savestr[LONGSTRING];
	char markedstr[MAXWORDSIZE];
	FILE * finput = NULL, *foutput = NULL;
	FILE * table_list = NULL;
	int maxstring = 0;
	int output_temporary = 0;
	size_t table_list_line = 0;
	
	if( is_deriv ) 
		dirp = DERIVTABLEDIR;
	else
		dirp = ENDTABLEDIR;
	
	endlines = (char **) calloc(MAX_END_TABLE,sizeof *endlines);
	if( ! endlines ) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
		return(-1);
	}
	if( table_list_path ) {
		table_list = fopen(table_list_path,"r");
		if( ! table_list ) {
			fprintf(stderr,"could not open table list: %s\n",table_list_path);
			goto failed;
		}
	}
	
	for(;;) {
		if( table_list ) {
			gk_string parsed = { 0 };
			gk_word word = { 0 };
			int list_status = read_table_name(
				table_list,listed_table,sizeof listed_table,&table_list_line);

			if( list_status < 0 )
				goto failed;
			if( list_status == 0 ) {
				if( fclose(table_list) == EOF ) {
					table_list = NULL;
					goto failed;
				}
				table_list = NULL;
				break;
			}
			curtable = listed_table;
			if( ScanAsciiKeys(curtable,&word,&parsed,NULL) < 0 )
				goto failed;
			if( is_deriv ) {
				if( ! derivtype_of(&parsed) || ! Is_regconj(&parsed) ) {
					fprintf(stderr,
					        "invalid derivation in table list at line %zu: %s\n",
					        table_list_line,curtable);
					goto failed;
				}
			} else {
				Stemtype listed_type = stemtype_of(&parsed);
				if( ! listed_type ) {
					fprintf(stderr,
					        "invalid ending in table list at line %zu: %s\n",
					        table_list_line,curtable);
					goto failed;
				}
				if( ! (listed_type & stype) )
					continue;
			}
		} else if( is_deriv ) {
			gk_string * gstring;
			gk_word * tmpGkword;
			Derivtype derivtype;
			int rconj;
		
			curtable = NextSuffTable(tmp);
			if( ! curtable ) {
				if( morpheus_runtime_context_current()->suffix_table_unavailable )
					goto failed;
				break;
			}
			nextkey(tmp,curderivname);
			curtable = curderivname;
			gstring = CreatGkString(1);
			tmpGkword = CreatGkword(1);
			if( ! gstring || ! tmpGkword ||
			    ScanAsciiKeys(curtable,tmpGkword,gstring,NULL) < 0 ) {
				if( gstring ) FreeGkString(gstring);
				if( tmpGkword ) FreeGkword(tmpGkword);
				goto failed;
			}
			derivtype = derivtype_of(gstring);
			rconj = Is_regconj(gstring);
			
			FreeGkString(gstring);
			FreeGkword(tmpGkword);
			if( ! rconj ) {
				printf("[%s] not a regular conj [%o] [%o]\n", curtable, derivtype, REG_DERIV);
				continue;
			}
		} else {
			curtable=NextEndTable(&index,stype);
			if( ! curtable &&
			    ! morpheus_runtime_context_current()->morph_keys_initialized )
				goto failed;
		}
		if( ! curtable ) break;
		
/*
		printf("about to compile [%s]\n", curtable );
*/
		if( snprintf(shortname,sizeof shortname,"%s%cout%c%s.out",
		             dirp, DIRCHAR, DIRCHAR, curtable) >= (int)sizeof shortname ) {
			fprintf(stderr,"ending-table path is too long: %s\n",curtable);
			goto failed;
		}

		if(! (finput=MorphFopen(shortname,"rb"))) {
			fprintf(stderr,"required ending table is missing: %s\n",shortname);
			goto failed;
		}
		Gstr = Blnk;	
		if( get_endheader(finput,&maxstring) < 0 || maxstring <= 0 ) {
			fprintf(stderr,"invalid ending table header: %s\n",shortname);
			goto failed;
		}

		while((read_status=ReadEnding(finput,&Gstr,maxstring)) > 0) {
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
			if( *sp && *sp != '*' && *(sp+strlen(sp)-1) == '*' )
				*(sp+strlen(sp)-1) = 0;
if(  ! *sp ) {
	printf("null ending in [%s]\n", curtable );
	continue;
}
			if( is_deriv ) {
				int written = snprintf(
					tmp,sizeof tmp,"%s\t%s",gkstring_of(&Gstr),
					strcmp(gkstring_of(&Gstr),savestr) ? savestr : "");
				if (written < 0 || (size_t)written >= sizeof tmp) {
					morpheus_runtime_error_record(
						MORPHEUS_RUNTIME_ERROR_INTERNAL);
					goto failed;
				}
				SprintGkFlags(&Gstr,tmp,sizeof tmp,":",0);
			} else {
				int written = snprintf(
					tmp,sizeof tmp,"%s\t%s",gkstring_of(&Gstr),
					NameOfStemtype(stemtype_of(&Gstr)));
				if (written < 0 || (size_t)written >= sizeof tmp) {
					morpheus_runtime_error_record(
						MORPHEUS_RUNTIME_ERROR_INTERNAL);
					goto failed;
				}
			}

/*
			if( is_deriv && derivtype_of(&Gstr) ) {
				strcat(tmp,":");
				strcat(tmp,NameOfDerivtype(derivtype_of(&Gstr)));
			}
*/
			if( endcount >= MAX_END_TABLE ) {
				fprintf(stderr,"more than %d endings in table! bye!\n", MAX_END_TABLE );
				goto failed;
			}
			*(endlines+endcount) = (char *)calloc(strlen(tmp)+1,sizeof ** endlines );
			if( ! *(endlines+endcount) ) {
				fprintf(stderr,"ran out of memory at %zu endings!\n", endcount );
				goto failed;
			}
			Xstrcpy(*(endlines+endcount),tmp);
			endcount++;
		}
/*
printf("deriv [%o] name [%s]\n", derivtype_of(&Gstr), 
NameOfDerivtype(derivtype_of(&Gstr)) );
*/
		if( read_status < 0 || ferror(finput) )
			goto failed;
		if( fclose(finput) == EOF ) {
			finput = NULL;
			goto failed;
		}
		finput = NULL;
	}

	qsort(endlines,endcount,sizeof * endlines, xstrcmp );
	
printf("stype [%o]\n", stype );
	if( is_deriv ) 
		basen = "derivind";
	else if( stype & PPARTMASK ) 
		basen = "vbendind";
	else
		basen = "nendind";
		
	i = snprintf(shortname,sizeof shortname,"%s%cindices%c%s",
	             dirp,DIRCHAR,DIRCHAR,basen);
	if (i < 0 || (size_t)i >= sizeof shortname) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		goto failed;
	}
	i = snprintf(temporary_shortname,sizeof temporary_shortname,"%s.tmp",
	             shortname);
	if (i < 0 || (size_t)i >= sizeof temporary_shortname) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		goto failed;
	}
	MorphPathName(shortname,output_path);
	MorphPathName(temporary_shortname,temporary_path);
	if( ! output_path[0] || ! temporary_path[0] )
		goto failed;

printf("output file:%s\n", shortname );
	if(! (foutput=MorphFopen(temporary_shortname,"w"))) {
		ErrorMess("Could not open nendind!");
		goto failed;
	}
	output_temporary = 1;
	
	prevtag[0] = 0;
	for(output_index=0;output_index<endcount;output_index++) {

		nextkey(*(endlines+output_index),curtag);
		/*
		 * if a new keys
		 */

		if( morphstrcmp(curtag,prevtag) ) {
			if( prevtag[0] && fprintf(foutput,"\n") < 0 )
				goto failed;
			if( fprintf(foutput,"%s%s%s",curtag,DELIMITER,
			            *(endlines+output_index)) < 0 )
				goto failed;
		} else if ( strcmp(prevkey,*(endlines+output_index) ) )
			/*
			 * don't include lines such as "uiais perf_act perf_act"
			 * where the same key is repeated
			 */
			if( fprintf(foutput,"%s%s",DELIMITER,
			            *(endlines+output_index)) < 0 )
				goto failed;
		Xstrcpy(prevtag,curtag);
		Xstrcpy(prevkey,*(endlines+output_index));
	}

	if( fprintf(foutput,"\n") < 0 || ferror(foutput) ||
	    fclose(foutput) == EOF ) {
		foutput = NULL;
		goto failed;
	}
	foutput = NULL;
	if( rename(temporary_path,output_path) != 0 )
		goto failed;
	output_temporary = 0;
/*	index_list(shortname,NULL);*/
	free_endlines(endlines,endcount);
	return(0);

failed:
	if( finput ) fclose(finput);
	if( foutput ) fclose(foutput);
	if( table_list ) fclose(table_list);
	if( output_temporary ) remove(temporary_path);
	free_endlines(endlines,endcount);
	return(-1);

}

static int
read_table_name(FILE *input, char *name, size_t name_size, size_t *line_number)
{
	size_t length;
	char *cursor;

	while( fgets(name,(int)name_size,input) ) {
		(*line_number)++;
		length = strlen(name);
		if( length && name[length-1] == '\n' )
			name[--length] = 0;
		else if( ! feof(input) ) {
			fprintf(stderr,"table-list line %zu is too long\n",*line_number);
			return(-1);
		}
		if( length && name[length-1] == '\r' )
			name[--length] = 0;
		if( ! length || name[0] == '#' )
			continue;
		for(cursor=name;*cursor;cursor++) {
			if( ! isalnum((unsigned char)*cursor) && *cursor != '_' ) {
				fprintf(stderr,"invalid table-list line %zu: %s\n",
				        *line_number,name);
				return(-1);
			}
		}
		return(1);
	}
	if( ferror(input) )
		return(-1);
	return(0);
}

static void
free_endlines(char **endlines, size_t endcount)
{
	size_t index;

	for(index=0;index<endcount;index++)
		free(endlines[index]);
	free(endlines);
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
