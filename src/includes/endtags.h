#ifndef MORPHEUS_ENDTAGS_H
#define MORPHEUS_ENDTAGS_H

#include <stdint.h>

#define MODULUS 10
#define KEYLEN 8
#define MAXTAG KEYLEN + 1

/*
 * Pre-index files store each text offset as an unsigned 32-bit little-endian
 * value. Keep this disk type independent from the host's long and pointer
 * widths so the same compiled stemlib is read on LP64 x86_64 and aarch64.
 */
typedef uint32_t morpheus_stemlib_offset;

typedef struct {
	char tagstring[MAXTAG];
	morpheus_stemlib_offset tagoffset;
} endtags;

#define tagstring_of(X) (X)->tagstring
#define tagoffset_of(X) (X)->tagoffset

endtags *init_preind(char *, int *);

#endif
