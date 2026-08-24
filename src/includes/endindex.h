#ifndef MORPHEUS_ENDINDEX_H
#define MORPHEUS_ENDINDEX_H

#ifndef MODULUS
#define MODULUS	10
#endif
#ifndef KEYLEN
#define KEYLEN 6
#endif
#ifndef MAXTAG
#define MAXTAG KEYLEN + 1
#endif

typedef struct endind {
	char *ebuf;
	char **eptr;
	int nelems;
} endind;

#define endbuffer_of(X) ((X)->ebuf)
#define endeptr_of(X) ((X)->eptr)
#define endlen_of(X) ((X)->nelems)

endind *init_endind(char *, endind *);

#endif
