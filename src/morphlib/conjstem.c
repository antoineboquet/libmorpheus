#include "morphlib_internal.h"
#include <ctype.h>
#include <gkstring.h>

#include "conjstem.proto.h"

static int
ends_with(const char *word, const char *suffix)
{
	size_t word_length = strlen(word);
	size_t suffix_length = strlen(suffix);

	return(word_length >= suffix_length &&
	       !strcmp(word+word_length-suffix_length,suffix));
}

static int
append_stem(char *stem, const char *suffix)
{
	if (strlen(stem) + strlen(suffix) >= MAXWORDSIZE) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}
	Xstrncat(stem,suffix,MAXWORDSIZE);
	return(1);
}

void fixcontr(char *stem, char *verb)
/* expand stem for contract verbs */
{
	if (!stem || !verb) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	if (ends_with(verb,"a/w") || ends_with(verb,"a/omai") ||
	    ends_with(verb,"e/w") || ends_with(verb,"e/omai"))
		append_stem(stem,"h");
	if (ends_with(verb,"o/w") || ends_with(verb,"o/omai"))
		append_stem(stem,"w");
}

void makeperf(char *s)
{
	register char *p;

	if (!s) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	if (!*s) return;
	p = s+strlen(s)-1;
	if (Is_dental(*p) || *p == 'z')
		*p = 'k';	 /* Smyth 560 */
	else if (Is_labial(*p) && *p != 'f')
		*p = 'f';			/* Smyth 569 */
	else if (Is_palatal(*p) && *p != 'x')
		*p = 'x';
	else if (*p == 'f' || *p == 'x')
		;		/* leave aspirated stops alone */
	else if (Is_liquid(*p) || Is_nasal(*p))
		;	/* Smyth 562 */
	else
		conjoin(s,"k");
}

void fixperf(char *s)
{
	register char *p;
	size_t length;

	if (!s) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	length = strlen(s);
	if (length < 2 || s[length-1] != 'k')
		return;
		
	p = s+length-2;
	if (Is_dental(*p) || *p == 'z') {
		*p = 'k';	 /* Smyth 560 */
		*(p+1) = 0;
	}
	else if (Is_labial(*p) && *p != 'f') {
		*p = 'f';			/* Smyth 569 */
		*(p+1) = 0;
	}
	else if (Is_palatal(*p) && *p != 'x') {
		*p = 'x';
		*(p+1) = 0;
	}
	else if (*p == 'f' || *p == 'x') {
		*(p+1) = 0;
	}		/* leave aspirated stops alone */
/*
 * grc 8/29/94
 *
 * no! leave as is
 * otherwise we get e)fqar-ka --> e)fa-ka
	 */
	else if (/*Is_liquid(*p) ||*/ Is_nasal(*p)) {
		*(p+1) = 0; /* Smyth 562 */
	}
}

void conjstem(char *stem, char *e)
{
	register char *p;
	char ending[MAXWORDSIZE];

	if (!stem || !e) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	if (!Xstrncpy(ending,e,MAXWORDSIZE)) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	if (!*stem) {
		Xstrncpy(stem,ending,MAXWORDSIZE);
		return;
	}
	p = stem+strlen(stem)-1;

	/* quick and dirty implementation of Smyth 405 */
	if (Is_cons(*p)) {
		if (0 == strcmp(ending,"ntai") )
			Xstrncpy(ending,"me/noi ei)si/{n}",MAXWORDSIZE);
		if (0 == strcmp(ending,"nto") ) {
			Xstrncpy(ending,"me/noi h)=san",MAXWORDSIZE);
			if (0 == Xstrncmp(stem,"e)",2)) {
				strsqz(stem,2);
				p -= 2;
				}
			}
		}

	if (0 == strcmp(ending,"hqi") && (*p == 'q'))	/* Smyth 125b */
		ending[1] = 't';

	conjoin(stem,ending);
}

void conjoin(char *stem, char *e)
{
	/* observe the laws of euphony, if you please... */
	register char *p;
	char ending[MAXWORDSIZE];
	int changed;
	size_t remaining;

	if (!stem || !e) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	if (!Xstrncpy(ending,e,MAXWORDSIZE)) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return;
	}
	if (!*ending) return;
	if (!*stem) {
		Xstrncpy(stem,ending,MAXWORDSIZE);
		return;
	}
	p = stem+strlen(stem)-1;
/*
 * this has got to be fixed if it is going to work at all well with
 * dialects!
 */
 
 /*
  * grc 4/21/89
  * an unbelievable kludge to allow me to generate plass- from plat-ss so that
  * i can get the epic form pla/ssa.  unbelievable! ugh.
	 */
 	if( (Is_dental(*p) || *p == 'z' || *p == 'n' ) && !Xstrncmp(e,"ss",2) ) {
		remaining = (size_t)MAXWORDSIZE-(size_t)(p-stem);
		if (strlen(ending) >= remaining) {
			morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
			return;
		}
		Xstrncpy(p,ending,remaining);
 		return;
 	}
 	
	do {
		changed = NO;
		switch(ending[0]) {
			case 's':
				changed = do_sigma(stem,ending);
				break;
			case 'q':
				changed = do_theta(stem);
				break;
			case 'm':
				changed = do_mu(stem);
				break;
			case 't':
				changed = do_tau(stem);
				break;
			}
		} while (changed);
	append_stem(stem,ending);
}

int do_sigma(char *s, char *ending)
{
	/* Smyth 537,545 */
	register char *p;
	int changed;

	if (!s || !ending) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(NO);
	}
	if (!*s || !*ending) return(NO);
	p = s+strlen(s)-1;
	changed = YES;

	if (*p == 'q' && ! strncmp(ending,"sk",2)) {
		*p = 's'; /* grc 9/3/94:  paq-skw --> pasxw */
		ending[1] = 'x';
	}
	if (ending[1] && Is_cons(*p) && Is_cons(ending[1]))
		strsqz(ending,1);	/* s between consonants drops out */
	else if (Is_labial(*p)) {
		*p = 'y';
		strsqz(ending,1);
		}
	else if (Is_palatal(*p)) {
		*p = 'c';
		strsqz(ending,1);
		}
/*
 * grc 9/12/88
 *
 * don't punt the 'r' or 'l' for two reasons:
 *
 *  a) a trashed 's' tends to lengthen the preceding syllable, e.g.
 *			estelsa --> esteila
 *
 *  b) this rule causes *amerdsa --> a)mesa, rather than --> a)mersa
 */ 
	else if (Is_dental(*p) || /*Is_liquid(*p) ||*/ Is_nasal(*p) || *p == 'z')
		*p = 0;	/* stops, liquids, etc. drop out Smyth 544*/
	else if (*p == 's' && *ending == 's')
		*p = 0; /* don't add 2nd s */
	else
		changed = NO;

	return (changed);
}

int do_theta(char *s)
{
	/* Smyth 587 */
	register char *p;
	int changed;

	if (!s) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(NO);
	}
	if (!*s) return(NO);
	changed = YES;
	p = s+strlen(s)-1;
	if (Is_labial(*p) && *p != 'f')
		*p = 'f';
	else if (Is_palatal(*p) && *p != 'x')
		*p = 'x';
	else if (Is_dental(*p) || *p == 'z')
		*p = 's';
	else
		changed = NO;
	return (changed);
}

int do_mu(char *s)
{
	/* Smyth 85 */
	register char *p;
	int changed;

	if (!s) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(NO);
	}
	if (!*s) return(NO);
	changed = YES;
	p = s+strlen(s)-1;
	if (Is_labial(*p)) {
		if (p > s && *(p-1) == 'm')
			*p = 0;
		else
			*p = 'm';
		}
	else if (Is_palatal(*p) && *p != 'g')
		*p = 'g';
	else if (Is_dental(*p) || *p == 'z')
		*p = 's';
	else
		changed = NO;
	return (changed);
}

int do_tau(char *s)
{
	/* Smyth 82-83 */
	register char *p;
	int changed;

	if (!s) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(NO);
	}
	if (!*s) return(NO);
	changed = YES;
	p = s+strlen(s)-1;
	if (Is_labial(*p) && *p != 'p')
		*p = 'p';
	else if (Is_palatal(*p) && *p != 'k')
		*p = 'k';
	else if (Is_dental(*p) || *p == 'z')
		*p = 's';
	else
		changed = NO;
	return (changed);
}
