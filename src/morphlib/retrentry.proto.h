
#include <stddef.h>

/* retrentry.c */
endtags *init_preind(char *, int *);
long ChckPreIndex(endtags *, char *, int, int,
                  int (*)(char *, char *));
int ChckFullIndex(char *, char *, char *, long,
                  int (*)(char *, char *, size_t));
