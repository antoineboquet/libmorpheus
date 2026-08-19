#ifndef MORPHEUS_CRUNCHER_INTERNAL_H
#define MORPHEUS_CRUNCHER_INTERNAL_H

#include <stdio.h>

#include <gkstring.h>
#include <prntflags.h>

/* Analyzer entry points used by the compatibility command-line client. */
int checkstring(char *string, PrntFlags prntflags, FILE *output);
char *anal_buf(void);
int show_totanals(void);
int show_totlems(void);

/* Runtime configuration and Beta Code normalisation helpers. */
int set_lang(int language);
int cur_lang(void);
int trimwhite(char *string);
int stripbreath(char *word);
int addbreath(char *word, int breathing);

#endif
