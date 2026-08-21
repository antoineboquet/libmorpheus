/* 
 * Greg Crane - Harvard
 * Jud Harward - Cincinnati
 * 1984-1985
 */
#include <greek.h>

#define DECALPHA 1
#include "vaxwords.proto.h"

/*
 * read in a 32 bit data word that has been written out to the disk on
 * a vax -- gets around different byte orders on different machines
 */
int get_int32(int32 *lword, FILE *f)
{
	
	int32 tmp;
	int i;
	int c;

	for(*lword=0,i=0;i<4;i++) {
		c = getc(f);
		if(c == EOF)
			return(0);
		tmp = (int32)c;
		tmp &= 0377;
		tmp = tmp << (8 * i);

		*lword += tmp;
	}
	return(1);
}

int put_int32(const int32 *lword, FILE *f)
{
	
	int32 tmp;
	int i;
	int c;

	for(i=0;i<4;i++) {
		tmp = *lword;
		tmp = tmp >> (8 * i);
		c = tmp & 0377;
		if(fputc(c , f ) == EOF)
			return(0);

	}
	return(1);
}

/*
 * read in a 16 bit data word that has been written out to the disk on
 * a vax -- gets around different byte orders on different machines
 */
int get_short(unsigned short *sword, FILE *f)
{
	unsigned int value = 0;
	unsigned int i;
	int c;

	for(i=0;i<2;i++) {
		c = getc(f);
		if(c == EOF)
			return(0);
		value |= (unsigned int)(c&0377) << (8U*i);
	}
	*sword = (unsigned short)value;
	return(1);
}

int put_short(const unsigned short *sword, FILE *f)
{
	unsigned int value = *sword;
	unsigned int i;
	int c;

	for(i=0;i<2;i++) {
		c = (int)((value >> (8U*i)) & 0377U);
		if(fputc(c , f ) == EOF)
			return(0);

	}
	return(1);
}

/*
 *	Routine converts reads shorts and longs written into a file in
 *	VAX byte order.
 *
 */


int vax_fread(void *Buffer, size_t size, int nswap, FILE *f)
{
    register int i;
    int32 * longp;
    unsigned short * shortp;
	size_t count;

	if(nswap < 0)
		return(-1);
	
	switch( size )  {

	    case 1:
					/* BYTES */
		count = fread(Buffer,size,(size_t)nswap,f);
		if(count < (size_t)nswap && ferror(f))
			return(-1);
		return((int)count);
	    case 2:			/* SHORTS */
		shortp = (unsigned short *)Buffer;

		for ( i=0; i < nswap; i++)  {
			if(!get_short(shortp,f))
				return(i == 0 && ferror(f) ? -1 : i);
			shortp++;
		}
		return(nswap);

	    case 4:			/* LONGS */

		longp = ( int32 *)Buffer;
		for ( i=0; i < nswap; i++)  {
			if(!get_int32(longp,f))
				return(i == 0 && ferror(f) ? -1 : i);
			longp++;
		}
		return(nswap);

		case 8: /* DOUBLES */
#ifdef DECALPHA
		longp = ( int32 *)Buffer;
		for ( i=0; i < nswap; i++)  {
			if(!get_int32(longp,f))
				return(i == 0 && ferror(f) ? -1 : i);
			longp++;
		}
		return(nswap);

#endif
		
	    default:

		fprintf(stderr, "vax_words: byte swap error, size = %zu\n", size);
		return(-1);
	}
}

int vax_fwrite(const void *Buffer, size_t size, int nswap, FILE *f)
{
    register int i;
	const int32 *longp;
	const unsigned short *shortp;
	size_t count;

	if(nswap < 0)
		return(-1);
	
	switch( size )  {
	    case 1:
					/* BYTES */
		count = fwrite(Buffer,size,(size_t)nswap,f);
		if(count < (size_t)nswap && ferror(f))
			return(-1);
		return((int)count);

	    case 2:			/* SHORTS */

		shortp = (const unsigned short *)Buffer;

		for ( i=0; i < nswap; i++)  {
			if(!put_short(shortp,f))
				return(i == 0 ? -1 : i);
			shortp++;
		}
		return(nswap);

	    case 4:			/* LONGS */

		longp = (const int32 *)Buffer;
		for ( i=0; i < nswap; i++)  {
			if(!put_int32(longp,f))
				return(i == 0 ? -1 : i);
			longp++;
		}
		return(nswap);

	    case 8:			/* DOUBLES */
#ifdef DECALPHA
		longp = (const int32 *)Buffer;
		for ( i=0; i < nswap; i++)  {
			if(!put_int32(longp,f))
				return(i == 0 ? -1 : i);
			longp++;
		}
		return(nswap);
#endif
	    default:

		fprintf(stderr, "vax_words: byte swap error, size = %zu\n", size);
		return(-1);

	}
}
