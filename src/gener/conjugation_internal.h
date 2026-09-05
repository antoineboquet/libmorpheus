/* SPDX-License-Identifier: MPL-2.0 */
#ifndef MORPHEUS_CONJUGATION_INTERNAL_H
#define MORPHEUS_CONJUGATION_INTERNAL_H
#include "../gkends/gkends_internal.h"
#include "../greeklib/greeklib_internal.h"
#include "../morphlib/morphlib_internal.h"
int GenConjForms(FILE * fin, FILE * fout, int conjmode);
void show_defvals(FILE * fout);
void set_newlemma(char * s);
int noppart(char * s);
int is_empty(char * s);
int has_pref(char * s, char * pref);
int need_ppart(char * s);
int check_vsdupl(char * s, FILE * fout);
int need_codupl(char * s);
int regular_entry(char * s);
int has_alpha(char * s);
void dummyfnc(void);
int irreg_conj(void);
int combine_conj(FILE * fout, char * lemma, char * origline, char * stemstr, char * derivstr, char * globalkeys, char * localkeys);
int DoConjStem(FILE * fout, char * derivstr, gk_string * gstr, char * suffstr, Stemtype ppartflag, char * stemstr, char * oddptr, char * preverb);
Stemtype ConjGkstr(gk_string * gstr, char * suffstr, char * globalkeys, char * keys, char * oddkeys, char * preverb);
int CheckConjPpart(FILE * fout, char * derivstr, gk_string * gstr1, gk_string * gstr2, char * stemstr, char * oddptr, char * preverb);
void add_oddstuff(char * s);
void DataBaseFormat(char * word, gk_string * gstr, char * endstring, char * preverb, char * oddptr);
void makeppass(char * origstem, gk_string * gstr);
int get_ppasstype(char * stem, char * newstem, char * stemname);
int MatchSuff(char *, char *);
void conjoinX(gk_string *, char *, char *);
extern FILE *morpheus_conj_odd_output;
_Noreturn void conj_fail(const char *);
void conj_copy(char *, const char *, size_t);
void conj_append(char *, const char *, size_t);
void conj_format(char *, size_t, const char *, ...);
void conj_key(char *, char *, size_t);
#endif
