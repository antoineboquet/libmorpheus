#include <greek.h>

int binlook(char *table, char *tag, int nelems, int size, bool exact_match,
            int (*compare)(char *, char *))
{
	int high;
	int low;
	int mid;
	int comp;
	int i;
	long offset;

	high = nelems-1;
	low = 0 ;


	while( low<=high ) {
		mid = (low+high)/2 ;
		offset = (long)(mid) * (long)size;

		comp = (*compare)( tag , (char *)table+offset );

		if( comp < 0 ) 
			high = mid - 1 ;
		else if ( comp > 0 )
			low = mid + 1;
		else  { /* found match */
 			return(mid);
		}
	}

	if( exact_match ) return(-1);
	if( mid > 0 ) {
		for(i=mid-1;i<nelems;i++) {
			offset = (long) size * (long)i;
			if( (*compare)(tag,table+offset) < 0 ) {
				break;
			}
		}
		if( i > 0 ) i--;
	} else
		i = 0;

	return( i );
}
