#ifndef MORPHEUS_MORPHSTRCMP_PROTO_H
#define MORPHEUS_MORPHSTRCMP_PROTO_H

#include <stddef.h>

int morphstrcmp(char *, char *);
int betastrcmp(char *, char *);
int morphstrncmp(char *, char *, size_t);
int dictstrcmp(char *, char *);
int dictstrncmp(char *, char *, size_t);
void init_comptab(void);
void init_betatab(void);

#endif
