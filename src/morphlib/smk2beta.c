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
	/*unsigned*/ char * s = start;
	
	context->inverse_conversion_current_font = 0;
	if( !context->inverse_conversion_tables_initialized ) {
		init_smk();
	}
	
	*result = 0;
	/*
	 * make sure that any unaccented upper case char gets properly converted
	 */
	if(isupper(*s) && context->inverse_conversion_from_smk) {
		Xstrcpy(tmp,s+1);
		*(s+1) = tolower(*s);
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
		
		if(isupper(*s) && ! context->inverse_conversion_from_smk ) {
			if( context->inverse_conversion_current_font == SMK_GREEK_FONT ||
				! context->inverse_conversion_current_font )
				set_cur_font(ROMAN,result);
			tmp[0] = tolower(*s);
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
		if( *s == '\253' && ! context->inverse_conversion_from_smk ) {
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
			if( isupper(*s) ) {
				tmp[0] = tolower(*s);
			} else
				tmp[0] = *s;
			tmp[0] = smk2betachar(tmp[0]);
			tmp[1] = 0;
			strcat(result,tmp);
			s++;	
		} else {
			if( ! context->inverse_conversion_from_smk ) {
				if( (context->inverse_smarta_characters[(int)(*s & (0377))] ||
					islower(*s)) &&
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

void init_smk(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	int i;

	for(i=0;i<MAXCHAR;i++) {
		context->smk_beta_table[i] = calloc(MAXSUBSTRING,1);
		context->smarta_beta_table[i] = calloc(MAXSUBSTRING,1);
		if (!context->smk_beta_table[i] ||
				!context->smarta_beta_table[i]) {
			fprintf(stderr,"could not allocate inverse conversion tables\n");
			exit(EXIT_FAILURE);
		}
	}
		
	for(i=0;i<sizeof Beta_SMK/sizeof Beta_SMK[0];i++) {
		strncpy(context->smk_beta_table[Beta_SMK[i].keycode],
			Beta_SMK[i].keystring,MAXSUBSTRING);
		strncpy(context->smarta_beta_table[Beta_SMK[i].keycode],
			Beta_SMK[i].keystring,MAXSUBSTRING);
		context->inverse_smarta_characters[Beta_SMK[i].keycode] = 1;
	}
	for(i=0;i<sizeof Beta_Smarta/sizeof Beta_Smarta[0];i++) {
		strncpy(context->smarta_beta_table[Beta_Smarta[i].keycode],
			Beta_Smarta[i].keystring,MAXSUBSTRING);
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
	
	if( isupper(*s) ) {
		if( !context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == SMK_GREEK_FONT ) {
			set_cur_font(ROMAN,res);
		}

		tmp[0] = '*';
		tmp[1] = tolower(*s);
		tmp[2] = 0;
		strcat(res,tmp);
		return;
	}
	
	if( islower(*s) ) {
		if( !context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == ROMAN ||
				context->inverse_conversion_current_font == ITALIC ) {
			set_cur_font(SMK_GREEK_FONT,res);
		}
		tmp[0] = '*';
		tmp[1] = *s;
		tmp[2] = 0;
		strcat(res,tmp);
		return;
	}

	tmp[0] = 0;
	if( SMK_ALPHA(*s) ) {
		add_acc(tmp, *s - ALPHA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"a");
	} else if( SMK_EPSILON(*s) ) {
		add_acc(tmp, *s - EPSILON_ACUTE + SPACE_ACUTE);
		strcat(tmp,"e");
	} else if( SMK_IOTA(*s) ) {
		add_acc(tmp, *s - IOTA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"i");
	} else if( SMK_OMICRON(*s) ) {
		add_acc(tmp, *s - OMICRON_ACUTE + SPACE_ACUTE);
		strcat(tmp,"o");
	} else if( SMK_UPSILON(*s) ) {
		add_acc(tmp, *s - UPSILON_ACUTE + SPACE_ACUTE);
		strcat(tmp,"u");
	} else if( SMK_ETA(*s) ) {
		add_acc(tmp, *s - ETA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"h");
	} else if( SMK_WMEGA(*s) ) {
		add_acc(tmp, *s - WMEGA_ACUTE + SPACE_ACUTE);
		strcat(tmp,"w");
	} else if( SMK_AISUB(*s) ) {
		add_acc(tmp, *s - AISUB_ACUTE + SPACE_ACUTE);
		strcat(tmp,"_");
		strcat(tmp,"a");
	} else if( SMK_EISUB(*s) ) {
		add_acc(tmp, *s - EISUB_ACUTE + SPACE_ACUTE);
		strcat(tmp,"_");
		strcat(tmp,"h");
	} else if( SMK_WISUB(*s) ) {
		add_acc(tmp, *s - WISUB_ACUTE + SPACE_ACUTE);
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
