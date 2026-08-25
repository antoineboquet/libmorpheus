/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_GENWD_PROTO_H
#define MORPHEUS_GENWD_PROTO_H

#include <gkstring.h>

void GenDictEntry(gk_word *, char *);
int GenNxtWord(FILE *, int, FILE *);
gk_word *GenStemForms(gk_word *, char *, int);
gk_word *GenIrregForm(gk_word *, char *, int);

#endif
