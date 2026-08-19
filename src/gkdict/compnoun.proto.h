#ifndef MORPHEUS_COMPNOUN_PROTO_H
#define MORPHEUS_COMPNOUN_PROTO_H

int checkforcompnoun(char *current_stem, char *ending_keys, char *stem_keys);
int setup_headtab(void);
int is_nomhead(char *heads, char *head_keys);

#endif
