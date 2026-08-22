#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "smk2beta.h"

#define MAXCHAR 		256
#define MAXSUBSTRING 	6
#define ROMAN 1
#define SMK_GREEK_FONT 2
#define ITALIC 3

#include "smk2beta.proto.h"
static void conv(char *, char *);
static void add_acc(char *, int);
static void clear_inverse_tables(morpheus_runtime_context *);

void smarta2beta(char *start, char *result)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	context->active_inverse_conversion_table = context->smarta_beta_table;
	
	context->inverse_conversion_from_smk = 0;
	
	conv(start,result);
}

void smk2beta(char *start, char *result)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	context->active_inverse_conversion_table = context->smk_beta_table;

	context->inverse_conversion_from_smk = 1;
	
	conv(start,result);
}

static
void conv(char *start, char *result)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	char **xlit_table = context->active_inverse_conversion_table;
	char tmp[BUFSIZ];
	/*unsigned*/ char *s;

	if (!result) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	*result = 0;
	if (!start) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	s = start;
	
	context->inverse_conversion_current_font = 0;
	if( !context->inverse_conversion_tables_initialized ) {
		if(!init_smk()) {
			return;
		}
	}
	
	/*
	 * make sure that any unaccented upper case char gets properly converted
	 */
	if(isupper((unsigned char)*s) && context->inverse_conversion_from_smk) {
		Xstrcpy(tmp,s+1);
		*(s+1) = (char)tolower((unsigned char)*s);
		*s = '*';
		Xstrcpy(s+2,tmp);
	}
	
	while(*s) {
/*
if( Xstrlen(result) > 256 ) {
printf("hey:%s\n", result);
*(result+256 ) = 0;
return;
}
*/
		if( *s == '^' && ! context->inverse_conversion_from_smk ) {
			s++;

			trap_upper(result,s);
			s++;
/*
			strcat(result,"*");
*/
			continue;
		}
		
		if(isupper((unsigned char)*s) && ! context->inverse_conversion_from_smk ) {
			if( context->inverse_conversion_current_font == SMK_GREEK_FONT ||
				! context->inverse_conversion_current_font )
				set_cur_font(ROMAN,result);
			tmp[0] = (char)tolower((unsigned char)*s);
			tmp[1] = 0;
			strcat(result,tmp);
			s++;
			continue;
		}
		if( *s == '_' && ! context->inverse_conversion_from_smk ) {
			set_cur_font(ITALIC,result);
			s++;
			continue;
		}
		if( (unsigned char)*s == 0253 && ! context->inverse_conversion_from_smk ) {
			set_cur_font(ROMAN,result);
			s++;
			continue;
		}
		if(*s == '`' && ! context->inverse_conversion_from_smk ) {
			if( ! context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == SMK_GREEK_FONT ) {
				set_cur_font(ROMAN,result);
			}
			strcat(result,":");
			s++;
			continue;
		}
		
		if( (*s & 0377) >= 0202 && (*s & 0377) <= 0212 ) {
			set_cur_font(SMK_GREEK_FONT,result);
			strcat(result,xlit_table[(int)(*s++ & (0377))]);
			
			/*
			 * we should only have an accented space (0202 thru 0212)
			 * if we have an upper case vowel following. in this case,
			 * we want to convert to a slighly different sequence for
			 * beta. 
			 *
			 * something like "\0205A" would be "*)/a" in beta
			 * transliteration
			 */
			if( isupper((unsigned char)*s) ) {
				tmp[0] = (char)tolower((unsigned char)*s);
			} else
				tmp[0] = *s;
			tmp[0] = (char)smk2betachar(tmp[0]);
			tmp[1] = 0;
			strcat(result,tmp);
			s++;	
		} else {
			if( ! context->inverse_conversion_from_smk ) {
				if( (context->inverse_smarta_characters[(unsigned char)*s] ||
					islower((unsigned char)*s)) &&
				  (context->inverse_conversion_current_font == ROMAN ||
				   context->inverse_conversion_current_font == ITALIC ||
				   ! context->inverse_conversion_current_font ) )
					set_cur_font(SMK_GREEK_FONT,result);
			}
			strcat(result,xlit_table[(int)(*s++ & (0377))]);
		}
	}
}

int smk2betachar(int c)
{
	if( c == 'v' ) return('w');
	if( c == 'y' ) return('q');
	if( c == 'c' ) return('y');
	if( c == 'j' ) return('c');
	if( c == 'W' ) return('v');
	return(c);
}

int init_smk(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	int i;

	if (context->inverse_conversion_tables_initialized)
		return(1);
	for(i=0;i<MAXCHAR;i++) {
		context->smk_beta_table[i] = calloc(MAXSUBSTRING,1);
		context->smarta_beta_table[i] = calloc(MAXSUBSTRING,1);
		if (!context->smk_beta_table[i] ||
				!context->smarta_beta_table[i]) {
			fprintf(stderr,"could not allocate inverse conversion tables\n");
			clear_inverse_tables(context);
			morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
			return(0);
		}
	}
		
	for(i=0;i<sizeof Beta_SMK/sizeof Beta_SMK[0];i++) {
		if (Beta_SMK[i].keycode < 0 || Beta_SMK[i].keycode >= MAXCHAR ||
		    strlen(Beta_SMK[i].keystring) >= MAXSUBSTRING)
			goto invalid_table;
		strcpy(context->smk_beta_table[Beta_SMK[i].keycode],
			Beta_SMK[i].keystring);
		strcpy(context->smarta_beta_table[Beta_SMK[i].keycode],
			Beta_SMK[i].keystring);
		context->inverse_smarta_characters[Beta_SMK[i].keycode] = 1;
	}
	for(i=0;i<sizeof Beta_Smarta/sizeof Beta_Smarta[0];i++) {
		if (Beta_Smarta[i].keycode < 0 ||
		    Beta_Smarta[i].keycode >= MAXCHAR ||
		    strlen(Beta_Smarta[i].keystring) >= MAXSUBSTRING)
			goto invalid_table;
		strcpy(context->smarta_beta_table[Beta_Smarta[i].keycode],
			Beta_Smarta[i].keystring);
	}
	
	for(i=0;i<256;i++) {
		if( ! *context->smk_beta_table[i] ) {
			sprintf(context->smk_beta_table[i] , "%c", i );
		}
		if( ! *context->smarta_beta_table[i] ) {
			sprintf(context->smarta_beta_table[i] , "%c", i );
		}
	}
	context->inverse_conversion_tables_initialized = 1;
	return(1);

invalid_table:
	fprintf(stderr,"invalid inverse conversion table entry\n");
	clear_inverse_tables(context);
	morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
	return(0);
}

static void
clear_inverse_tables(morpheus_runtime_context *context)
{
	int i;

	for (i=0;i<MAXCHAR;i++) {
		free(context->smk_beta_table[i]);
		free(context->smarta_beta_table[i]);
		context->smk_beta_table[i] = NULL;
		context->smarta_beta_table[i] = NULL;
		context->inverse_smarta_characters[i] = 0;
	}
	context->active_inverse_conversion_table = NULL;
	context->inverse_conversion_tables_initialized = 0;
}

void set_cur_font(int n, char *s)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	if( context->inverse_conversion_from_smk ) return;
	
	if( n != context->inverse_conversion_current_font ) {
		switch(n) {
			case SMK_GREEK_FONT:
				strcat(s,"$");
				break;
			case ROMAN:
				strcat(s,"&");
				break;
			case ITALIC:
				strcat(s,"&3");
				break;
			default:
				strcat(s,"?Font?");
				break;
			}
		context->inverse_conversion_current_font = n;
		}
}
#define SPACE_ACUTE 0200
#define ALPHA_ACUTE 0213
#define EPSILON_ACUTE 0241
#define IOTA_ACUTE 0333
#define OMICRON_ACUTE 0361	
#define UPSILON_ACUTE 0346
#define ETA_ACUTE 0256
#define WMEGA_ACUTE 0305
#define AISUB_ACUTE 0226
#define EISUB_ACUTE 0372
#define WISUB_ACUTE 0304

void trap_upper(char *res, char *s)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	char tmp[BUFSIZ];
	unsigned int byte = (unsigned char)*s;
	
	if( isupper((int)byte) ) {
		if( !context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == SMK_GREEK_FONT ) {
			set_cur_font(ROMAN,res);
		}

		tmp[0] = '*';
		tmp[1] = (char)tolower((int)byte);
		tmp[2] = 0;
		strcat(res,tmp);
		return;
	}
	
	if( islower((int)byte) ) {
		if( !context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == ROMAN ||
				context->inverse_conversion_current_font == ITALIC ) {
			set_cur_font(SMK_GREEK_FONT,res);
		}
		tmp[0] = '*';
		tmp[1] = (char)byte;
		tmp[2] = 0;
		strcat(res,tmp);
		return;
	}

	tmp[0] = 0;
	if( SMK_ALPHA(byte) ) {
		add_acc(tmp, (int)byte - ALPHA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"a");
	} else if( SMK_EPSILON(byte) ) {
		add_acc(tmp, (int)byte - EPSILON_ACUTE + SPACE_ACUTE);
		strcat(tmp,"e");
	} else if( SMK_IOTA(byte) ) {
		add_acc(tmp, (int)byte - IOTA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"i");
	} else if( SMK_OMICRON(byte) ) {
		add_acc(tmp, (int)byte - OMICRON_ACUTE + SPACE_ACUTE);
		strcat(tmp,"o");
	} else if( SMK_UPSILON(byte) ) {
		add_acc(tmp, (int)byte - UPSILON_ACUTE + SPACE_ACUTE);
		strcat(tmp,"u");
	} else if( SMK_ETA(byte) ) {
		add_acc(tmp, (int)byte - ETA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"h");
	} else if( SMK_WMEGA(byte) ) {
		add_acc(tmp, (int)byte - WMEGA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"w");
	} else if( SMK_AISUB(byte) ) {
		add_acc(tmp, (int)byte - AISUB_ACUTE + SPACE_ACUTE);
		strcat(tmp,"_");
		strcat(tmp,"a");
	} else if( SMK_EISUB(byte) ) {
		add_acc(tmp, (int)byte - EISUB_ACUTE + SPACE_ACUTE);
		strcat(tmp,"_");
		strcat(tmp,"h");
	} else if( SMK_WISUB(byte) ) {
		add_acc(tmp, (int)byte - WISUB_ACUTE + SPACE_ACUTE);
		strcat(tmp,"_");
		strcat(tmp,"w");
	}
	if( tmp[0] ) {
		if( ! context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == ROMAN ||
				context->inverse_conversion_current_font == ITALIC )
			set_cur_font(SMK_GREEK_FONT,res);
		strcat(res,tmp);
	}
}

static 
void add_acc(char *s, int anum)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	Xstrcpy(s,context->active_inverse_conversion_table[
		(int)( anum & (0377))]);
}
