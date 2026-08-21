#include "morphlib_internal.h"
#include <gkstring.h>
#include <smk.h>
#define ACUTEFLAG 		01
#define GRAVEFLAG 		02
#define CIRCUMFLAG 		04
#define SMOOTHFLAG 		010
#define	ROUGHFLAG		020
#define LONGMARK		040
#define DIAERFLAG		0100
#define SHORTFLAG		0200
#define ISUBFLAG		0400
#define EQUALS   		'='
#define UCASEMARKER		'^'
#define SMARTA_ROUGH_RHO		0373
#define SMK_ROUGH_RHO		075
#define TERMINAL_SIGMA	'w'
#define SMARTA_GREEK_FONT 	0100
#define ROMAN 			0200
#define AISUB			046
#define HISUB			0372
#define WISUB			0304
#define IS_CHARSTYLE(S) (*S == '&' && (*(S+1) == '3' || *(S+1) == '1' ))
#define END_CHARSTYLE(S) if (xlit==SMARTA) 	*S++ = byte_value(0253);/* \
else { Xstrcpy(S,"}"), S ++; }*/

#define GKFONT "}{\\f132 "
#define ROMANFONT "}{"
#define BOLDFONT "}{\\b "
#define ITALICFONT "}{\\ulw "
#define ITALIC '3'
#define BOLD '1'

#include "beta2smarta.proto.h"
static void init_gktab(morpheus_runtime_context *);
static char byte_value(unsigned int);
static const int acctab[] = {
	ACUTEFLAG,
	GRAVEFLAG,
	CIRCUMFLAG,
	SMOOTHFLAG,
	ROUGHFLAG,
	SMOOTHFLAG|ACUTEFLAG,
	ROUGHFLAG|ACUTEFLAG,
	SMOOTHFLAG|GRAVEFLAG,
	ROUGHFLAG|GRAVEFLAG,
	SMOOTHFLAG|CIRCUMFLAG,
	ROUGHFLAG|CIRCUMFLAG
};

static 
void init_gktab(morpheus_runtime_context *context)
{
	int *accenttab = context->smarta_accent_table;
	int *gktab = context->smarta_greek_table;
	
	context->smarta_tables_initialized = 1;
	
	accenttab['/'] = ACUTEFLAG;
	accenttab['\\'] = GRAVEFLAG;
	accenttab['='] = CIRCUMFLAG;
	accenttab[')'] = SMOOTHFLAG;
	accenttab['('] = ROUGHFLAG;
	accenttab[HARDLONG] = LONGMARK;
	accenttab['+'] = DIAERFLAG;
	accenttab[HARDSHORT] = SHORTFLAG;
	accenttab['|'] = ISUBFLAG;
	
	gktab[' '] = 0200;
	gktab['a'] = 0213;
	gktab['e'] = 0241;
	gktab['h'] = 0256;
	/* note that 'v' is SMK for omega (beta code 'w'). */
	gktab['v'] = 0305;
	gktab['i'] = 0333;
	gktab['u'] = 0346;
	gktab['o'] = 0361;
	gktab[AISUB] = 0226;
	gktab[WISUB] = 0320;
	gktab[HISUB] = 0271;
}

static char byte_value(unsigned int value)
{
	return (char)(unsigned char)value;
}

#define Is_accflag(X) \
	(accenttab[(unsigned char)(X)] > 0 && \
	 accenttab[(unsigned char)(X)] <= ISUBFLAG)
#define SMARTA 2
#define SMK 4

void beta2smarta(char *source, char *res)
{
	beta2mac(source,res,SMARTA);
}

void beta2smk(char *source, char *res)
{
	beta2mac(source,res,SMK);
}

void set_greek(void)
{
	morpheus_runtime_context_current()->smarta_current_font = SMARTA_GREEK_FONT;
}

void set_roman(void)
{
	morpheus_runtime_context_current()->smarta_current_font = ROMAN;
}

void beta2mac(char *source, char *res, int xlit)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	int *accenttab = context->smarta_accent_table;
	int *gktab = context->smarta_greek_table;
	 char * sp;
	/*unsigned*/ char * rp;
	int acc;
	int saw_isub = 0;
	int long_vowel = 0;
	
	if( ! context->smarta_current_font )
		context->smarta_current_font = SMARTA_GREEK_FONT;
	if( ! context->smarta_tables_initialized ) init_gktab(context);
	
	sp = source; rp = res;
	
	while(*sp) {
		if( *sp == '$' ) {
			if( context->smarta_character_style ) {
				if( rp == res ) {
					END_CHARSTYLE(rp);
/*					*rp++ = 0253;*/
				} else {
					if( rp > res) rp--;
/* grc 6/26/89
					while(*rp == ' ' && rp > res ) rp--;
*/
					if( ispunct((unsigned char)*rp) ) {
						*(rp+1) = *rp;
						END_CHARSTYLE(rp);
/*						*rp = 0253;
						rp++; 
*/
						rp++;
					} else {
						rp++;
						END_CHARSTYLE(rp);
/*						*rp++ = 0253;*/
					}
/*
					*rp++ = ' ';
*/
				}
				context->smarta_character_style = 0;
			}
			sp = greekfont(sp);
			if( xlit == SMK ) {
				Xstrcpy(rp,GKFONT); 
				rp += Xstrlen(GKFONT);
			}
			continue;
		} else if( *sp == '&' ) {

			if( context->smarta_character_style && ! IS_CHARSTYLE(sp)) {
				END_CHARSTYLE(rp);
				context->smarta_character_style = 0;
			}
			if( IS_CHARSTYLE(sp) ) {
				if( (*(sp+1) == '3' && context->smarta_character_style == BOLD ) &&
					(*(sp+1) == '1' && context->smarta_character_style == ITALIC ) ) {
						END_CHARSTYLE(rp);
						context->smarta_character_style = 0;
				}
				if( *(sp+1) == '3' )
					context->smarta_character_style = ITALIC;
				else if( *(sp+1) == '1' )
					context->smarta_character_style = BOLD;
/*
				*rp++ = ' ';
*/
				if( xlit == SMARTA ) 
					*rp++ = 0137;
				else {
					if( context->smarta_character_style == ITALIC ) {
						Xstrcpy(rp,ITALICFONT); rp += Xstrlen(ITALICFONT);
					} else {
						Xstrcpy(rp,BOLDFONT); rp += Xstrlen(BOLDFONT);
					}
				}
				sp += 2;
				while(isspace((unsigned char)*sp)) sp++;
			}
			sp = romanfont(sp);
			if( xlit == SMK && ! context->smarta_character_style && *(rp-1) != '}' ) {
				Xstrcpy(rp,ROMANFONT);  rp += Xstrlen(ROMANFONT);
			}
			continue;
		}
			
		if( *sp == '%' ) {
			int n;
			char numbuf[8];
			char * np;
			
			np = numbuf;
			n = atoi(++sp);
			while(isdigit((unsigned char)*sp)) *np++ = *sp++;
			*np = 0;
			
			switch(n) {
				case 1:
					*rp++ = '?';
					break;
				case 2:
					*rp++ = '*';
					break;
				case 4:
					*rp++ = '!';
					break;
				case 6:
					if( xlit == SMARTA ) 
						*rp++ = EQUALS;
					else {
						if( context->smarta_current_font == SMARTA_GREEK_FONT ) {
							Xstrcpy(rp,ROMANFONT);
							rp += Xstrlen(ROMANFONT);
							Xstrcpy(rp,"=}{");
							rp += 3;
							Xstrcpy(rp,GKFONT);
							rp += Xstrlen(GKFONT);
						} else
							*rp++ = '=';
					}
					break;
				case 10:
					if( xlit == SMARTA )
						*rp++ = '`';
					else
						*rp++ = ':';
					break;
				case 40:
					if( xlit == SMK ) {
						*rp++ = ' ';
						*rp++ = byte_value(SMK_SHORTMARK);
					} else if( xlit == SMARTA ) {
						*rp++ = byte_value(SMARTA_SHORTMARK);
						break;
					}
				case 41:
					if( xlit == SMK ) {
						*rp++ = ' ';
						*rp++ = SMK_LONGMARK;
						break;
					} else if( xlit == SMARTA ) {
						*rp++ = SMARTA_LONGMARK;
					}
					
				default:
					np = numbuf;
					*rp ++ = '%';
					while(*np) *rp++ = *np++;
					break;
				}
			continue;
		}
		if( *sp == '*' && context->smarta_current_font == ROMAN ) {
			if( xlit == SMARTA ) {
				*rp++ = UCASEMARKER;
			} else if (xlit == SMK ) {
				sp++;
				Xstrcpy(rp,sp);
				if( islower((unsigned char)*rp) )
					*rp = (char)toupper((unsigned char)*rp);
				rp++;
			}
			sp++;
			continue;
		}
		if( context->smarta_current_font == ROMAN &&
				isalpha((unsigned char)*sp) ) {
			if( isupper((unsigned char)*sp) ) {
				*rp++ = UCASEMARKER;
				*rp++ = *sp++;
				continue;
			} else {
				if( xlit == SMARTA ) 
					*rp++ = (char)toupper((unsigned char)*sp++);
				else
					*rp++ = *sp++;
			}
			continue;
		}
		if( *sp == '[' && *(sp+1) == '1' && *(sp+2) != '.' ) {
				/* 
				 * grc 6/5/88
				 *
				 * note that we want to accept things like "[1.]" which show up
				 * all the time in the Greek English Lexicon.
				 */
				*rp++ = '(';
				sp += 2;
				continue;
		}
		
		if( *sp == ']' && *(sp+1) == '1') {
				*rp++ = ')';
				sp += 2;
				continue;
		}
		
		if(isalpha((unsigned char)*sp) || *sp == '*') {
			acc = 0;
			
			
			if( xlit == SMK && *sp == '*' ) {
				if( Is_accflag(*(sp+1)) ) {
					 char * t = sp;
					*sp = ' ';
					while(*t&&!isalpha((unsigned char)*t)) t++;
					if(isalpha((unsigned char)*t) &&
							islower((unsigned char)*t) )
						*t = (char)toupper((unsigned char)*t);
				} else {
					Xstrcpy(sp,sp+1);
					if(islower((unsigned char)*sp))
						*sp = (char)toupper((unsigned char)*sp);
				}
			} 
			

			*rp = *sp++;
			
			
			if( isupper((unsigned char)*rp) && xlit == SMARTA ) {
					*(rp+1) = (char)tolower((unsigned char)*rp);
					*rp++ = UCASEMARKER;
			}

/*			if( *rp == '*' ) {
				if( xlit == SMARTA )
					*rp = UCASEMARKER;
				*sp = smk_char_xlit(*sp,sp+1);
			} else 
*/
				*rp = (char)smk_char_xlit(*rp,sp,xlit);
/*			
			if( *rp == 's' && !isalpha(*sp) && *sp != '\'' && *sp != '-' )
				*rp = TERMINAL_SIGMA;
			else if( *rp == 'w' )
				*rp = 'v';
			else if( *rp == 'q' )
				*rp = 'y';
			else if( *rp == 'Q' )
				*rp = 'Y';
			else if( *rp == 'c' )
				*rp = 'j';
			else if( *rp == 'C' )
				*rp = 'J';
			else if (*rp == 'y' )
				*rp = 'c';
			else if (*rp == 'W' )
				*rp = 'V';
			else if (*rp == 'V' )
				*rp = 'C';
			else if( *rp == 'v' ) /* digamma *
				*rp = 'W';
			else if( *rp == '*' && xlit == SMARTA ) {
					*rp = UCASEMARKER;
			} /*else if ( xlit == SMK && ) {
					unsigned char * t = sp;
					if( Is_accflag(*sp)) {
							*rp = ' ';
						while(*t&&!isalpha(*t)) t++;
						if(isalpha(*t)&&islower(*t)) *t = toupper(*t);
					} else {
						*rp = toupper(*sp++);
					}
				}
					
			}*/
			
			while( Is_accflag(*sp) ) {

				if( *sp == HARDLONG ) {
					long_vowel++;
					sp++;
				} else if( *sp == '|' )  {
					saw_isub++;
					sp++;
				} else if (*sp == HARDSHORT)
					sp++;
				/*
				 * don't count the hard short marker (no way to print it for now)
				 */
				 else		
					acc += accenttab[(unsigned char)*sp++];
			}
/*
printf("got [%o] ", acc );
*/
/*
			if( *sp == '|' ) {
				saw_isub++;
				sp++;
			}
*/
			/*
			 * if you see a capital letter marker 
			 * then keep that and find the letter that that
			 * capital would cover.
			 */
			if( acc && *rp == UCASEMARKER && xlit == SMARTA ) {
				if( isalpha((unsigned char)*sp ) ) {
					*++rp = *sp++;
					*rp = (char)smk_char_xlit(*rp,sp,xlit);
				}
			} 
			
			/*
			 * don't bother showing the long mark if the letter is
			 * accented with a circumflex
			 */
			if( long_vowel )  {
				if( ! (acc & CIRCUMFLAG ) ) {
					if(xlit == SMARTA ) {
						if( *rp == 'a' )
							*rp = 046;
						else if( *rp == 'i' ) {
							*rp = byte_value(0372);
						} else if( *rp == 'u' ) {
							*rp = byte_value(0304);
						}
					} else if( xlit == SMK ) {
						*(rp+1) = *rp;
						*rp++ = '*';
					}
				} 
				long_vowel = 0;
			}
			if( saw_isub && xlit == SMK ) {
				switch(*rp) {
					case 'a':
						*rp = AISUB;
						break;
					case 'h':
						*rp = byte_value(HISUB);
						break;
					case 'v':
						*rp = byte_value(WISUB);
						break;
					default:
						break;
				}
				saw_isub = 0;
			} 
				
			
			if( acc ) {
				if(*rp == 'r' && acc == ROUGHFLAG ) {
					if( xlit == SMK ) 
						*rp = SMK_ROUGH_RHO;
					else
						*rp = byte_value(SMARTA_ROUGH_RHO);
				} else if( acc == DIAERFLAG  &&
							(*rp == 'i' || *rp == 'u') ) {
					if( *rp == 'i' ) 
						*rp = byte_value(0363);
					else 
						*rp =  043;
				} else if( acc == (DIAERFLAG|ACUTEFLAG)  &&
							(*rp == 'i' || *rp == 'u') ) {
					if( *rp == 'i' ) 
						*rp = byte_value(0375);
					else 
						*rp =  0100;
				}else if( acc == (DIAERFLAG|GRAVEFLAG)  &&
							(*rp == 'i' || *rp == 'u') ) {
					if( *rp == 'i' ) 
						*rp = byte_value(0376);
					else 
						*rp = byte_value(0243);
				} else if( !gktab[(unsigned char)*rp] ) {
					*(rp+1) = *rp;
					*rp = '?';
					rp += 2;
					*rp = '?';
				} else
					*rp = byte_value((unsigned int)(gktab[(unsigned char)*rp] + accnum(acc)));
/*
if(1) {
int n;
n = *rp;
n &= 0377;
printf(" *rp [%o] n [%o] ", *rp , n  );
}
*/
			}
			if( saw_isub && xlit == SMARTA ) {
				saw_isub = 0;
				*++rp = 'i';
			}
		}  else if( *sp == '_' ) {
			*rp = '-';
			sp++;
		} else
			*rp = *sp++;
		rp++;
	}
	*rp = 0;
}

int accnum(int n)
{
	int i;
	
	for(i=0;i<sizeof acctab/sizeof acctab[0];i++ )
		if( n == acctab[i] )
			return(i);
	return(0);
}

char * 
romanfont(char *s)
{
	morpheus_runtime_context_current()->smarta_current_font = ROMAN;
	while(*s && *s=='&') s++;
	if( isdigit((unsigned char)*s) )
		while(isdigit((unsigned char)*s)) s++;
	else if( *s == ' '&& *(s+1) == ' ' ) s++;
	return(s);
}

char * 
greekfont(char *s)
{
	morpheus_runtime_context_current()->smarta_current_font = SMARTA_GREEK_FONT;
	while(*s && *s=='$') s++;
	if( isdigit((unsigned char)*s) )
		while(isdigit((unsigned char)*s)) s++;
	else if( *s == ' ' && *(s+1) == ' '  ) s++;
	return(s);
}

int smk_char_xlit(int c, char *s, int xlit)
{
			if( c == 's' && !isalpha((unsigned char)*s) &&
					*s != '\'' && *s != '-' )
				c = TERMINAL_SIGMA;
			else if( c == 'w' )
				c = 'v';
			else if( c == 'q' )
				c = 'y';
			else if( c == 'Q' )
				c = 'Y';
			else if( c == 'c' )
				c = 'j';
			else if( c == 'C' )
				c = 'J';
			else if (c == 'y' )
				c = 'c';
			else if (c == 'W' )
				c = 'V';
			else if (c == 'V' )
				c = 'C';
			else if( c == 'v' ) /* digamma */
				if( xlit == SMARTA ) 
					c = 'q';
				else c = 'W';
			else if( c == '*' && xlit == SMARTA ) 
				c = UCASEMARKER;

			return(c);
}
