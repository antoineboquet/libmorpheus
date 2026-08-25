/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_COMPNOUN_PROTO_H
#define MORPHEUS_COMPNOUN_PROTO_H

#include <stdio.h>

void checkforcompnoun(char *current_stem, char *ending_keys, char *stem_keys);
int setup_headtab(void);
int setup_headtab_stream(FILE *head_stream);
int is_nomhead(char *heads, char *head_keys);

#endif
