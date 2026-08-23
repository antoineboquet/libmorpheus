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
static int conv(char *, char *, size_t);
static int add_acc(char *, size_t, int);
static void clear_inverse_tables(morpheus_runtime_context *);

int smarta2beta(char *start, char *result, size_t capacity)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	context->active_inverse_conversion_table = context->smarta_beta_table;
	
	context->inverse_conversion_from_smk = 0;
	
	return(conv(start,result,capacity));
}

int smk2beta(char *start, char *result, size_t capacity)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	context->active_inverse_conversion_table = context->smk_beta_table;

	context->inverse_conversion_from_smk = 1;
	
	return(conv(start,result,capacity));
}

static
int conv(char *start, char *result, size_t capacity)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	char **xlit_table = context->active_inverse_conversion_table;
	char tmp[BUFSIZ];
	/*unsigned*/ char *s;

	if (!result || !capacity) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}
	*result = 0;
	if (!start) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}
	s = start;
	
	context->inverse_conversion_current_font = 0;
	if( !context->inverse_conversion_tables_initialized ) {
		if(!init_smk()) {
			return(0);
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

			if (!trap_upper(result,capacity,s)) goto too_long;
			s++;
/*
			strcat(result,"*");
*/
			continue;
		}
		
		if(isupper((unsigned char)*s) && ! context->inverse_conversion_from_smk ) {
			if( context->inverse_conversion_current_font == SMK_GREEK_FONT ||
				! context->inverse_conversion_current_font )
				if (!set_cur_font(ROMAN,result,capacity)) goto too_long;
			tmp[0] = (char)tolower((unsigned char)*s);
			tmp[1] = 0;
			if (!Xstrncat(result,tmp,capacity)) goto too_long;
			s++;
			continue;
		}
		if( *s == '_' && ! context->inverse_conversion_from_smk ) {
			if (!set_cur_font(ITALIC,result,capacity)) goto too_long;
			s++;
			continue;
		}
		if( (unsigned char)*s == 0253 && ! context->inverse_conversion_from_smk ) {
			if (!set_cur_font(ROMAN,result,capacity)) goto too_long;
			s++;
			continue;
		}
		if(*s == '`' && ! context->inverse_conversion_from_smk ) {
			if( ! context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == SMK_GREEK_FONT ) {
				if (!set_cur_font(ROMAN,result,capacity)) goto too_long;
			}
			if (!Xstrncat(result,":",capacity)) goto too_long;
			s++;
			continue;
		}
		
		if( (*s & 0377) >= 0202 && (*s & 0377) <= 0212 ) {
			if (!set_cur_font(SMK_GREEK_FONT,result,capacity) ||
			    !Xstrncat(result,xlit_table[(int)(*s++ & (0377))],capacity))
				goto too_long;
			
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
			if (!Xstrncat(result,tmp,capacity)) goto too_long;
			s++;	
		} else {
			if( ! context->inverse_conversion_from_smk ) {
				if( (context->inverse_smarta_characters[(unsigned char)*s] ||
					islower((unsigned char)*s)) &&
				  (context->inverse_conversion_current_font == ROMAN ||
				   context->inverse_conversion_current_font == ITALIC ||
				   ! context->inverse_conversion_current_font ) )
					if (!set_cur_font(SMK_GREEK_FONT,result,capacity))
						goto too_long;
			}
			if (!Xstrncat(result,xlit_table[(int)(*s++ & (0377))],capacity))
				goto too_long;
		}
	}
	return(1);

too_long:
	*result = 0;
	morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
	return(0);
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
		Xstrncpy(context->smk_beta_table[Beta_SMK[i].keycode],
			Beta_SMK[i].keystring,MAXSUBSTRING);
		Xstrncpy(context->smarta_beta_table[Beta_SMK[i].keycode],
			Beta_SMK[i].keystring,MAXSUBSTRING);
		context->inverse_smarta_characters[Beta_SMK[i].keycode] = 1;
	}
	for(i=0;i<sizeof Beta_Smarta/sizeof Beta_Smarta[0];i++) {
		if (Beta_Smarta[i].keycode < 0 ||
		    Beta_Smarta[i].keycode >= MAXCHAR ||
		    strlen(Beta_Smarta[i].keystring) >= MAXSUBSTRING)
			goto invalid_table;
		Xstrncpy(context->smarta_beta_table[Beta_Smarta[i].keycode],
			Beta_Smarta[i].keystring,MAXSUBSTRING);
	}
	
	for(i=0;i<256;i++) {
		if( ! *context->smk_beta_table[i] ) {
			context->smk_beta_table[i][0] = (char)i;
			context->smk_beta_table[i][1] = 0;
		}
		if( ! *context->smarta_beta_table[i] ) {
			context->smarta_beta_table[i][0] = (char)i;
			context->smarta_beta_table[i][1] = 0;
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

int set_cur_font(int n, char *s, size_t capacity)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	if( context->inverse_conversion_from_smk ) return(1);
	
	if( n != context->inverse_conversion_current_font ) {
		switch(n) {
			case SMK_GREEK_FONT:
				if (!Xstrncat(s,"$",capacity)) return(0);
				break;
			case ROMAN:
				if (!Xstrncat(s,"&",capacity)) return(0);
				break;
			case ITALIC:
				if (!Xstrncat(s,"&3",capacity)) return(0);
				break;
			default:
				if (!Xstrncat(s,"?Font?",capacity)) return(0);
				break;
			}
		context->inverse_conversion_current_font = n;
		}
	return(1);
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

int trap_upper(char *res, size_t capacity, char *s)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	char tmp[BUFSIZ];
	unsigned int byte = (unsigned char)*s;
	
	if( isupper((int)byte) ) {
		if( !context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == SMK_GREEK_FONT ) {
			if (!set_cur_font(ROMAN,res,capacity)) return(0);
		}

		tmp[0] = '*';
		tmp[1] = (char)tolower((int)byte);
		tmp[2] = 0;
		return(Xstrncat(res,tmp,capacity));
	}
	
	if( islower((int)byte) ) {
		if( !context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == ROMAN ||
				context->inverse_conversion_current_font == ITALIC ) {
			if (!set_cur_font(SMK_GREEK_FONT,res,capacity)) return(0);
		}
		tmp[0] = '*';
		tmp[1] = (char)byte;
		tmp[2] = 0;
		return(Xstrncat(res,tmp,capacity));
	}

	tmp[0] = 0;
	if( SMK_ALPHA(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - ALPHA_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"a",sizeof tmp)) return(0);
	} else if( SMK_EPSILON(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - EPSILON_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"e",sizeof tmp)) return(0);
	} else if( SMK_IOTA(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - IOTA_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"i",sizeof tmp)) return(0);
	} else if( SMK_OMICRON(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - OMICRON_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"o",sizeof tmp)) return(0);
	} else if( SMK_UPSILON(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - UPSILON_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"u",sizeof tmp)) return(0);
	} else if( SMK_ETA(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - ETA_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"h",sizeof tmp)) return(0);
	} else if( SMK_WMEGA(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - WMEGA_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"w",sizeof tmp)) return(0);
	} else if( SMK_AISUB(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - AISUB_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"_a",sizeof tmp)) return(0);
	} else if( SMK_EISUB(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - EISUB_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"_h",sizeof tmp)) return(0);
	} else if( SMK_WISUB(byte) ) {
		if (!add_acc(tmp,sizeof tmp,(int)byte - WISUB_ACUTE + SPACE_ACUTE) ||
		    !Xstrncat(tmp,"_w",sizeof tmp)) return(0);
	}
	if( tmp[0] ) {
		if( ! context->inverse_conversion_current_font ||
				context->inverse_conversion_current_font == ROMAN ||
				context->inverse_conversion_current_font == ITALIC )
			if (!set_cur_font(SMK_GREEK_FONT,res,capacity)) return(0);
		if (!Xstrncat(res,tmp,capacity)) return(0);
	}
	return(1);
}

static 
int add_acc(char *s, size_t capacity, int anum)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	return(Xstrncpy(s,context->active_inverse_conversion_table[
		(int)( anum & (0377))],capacity));
}
