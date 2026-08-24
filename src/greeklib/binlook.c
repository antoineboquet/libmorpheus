#include <greek.h>
#include <stdint.h>

int binlook(char *table, char *tag, int nelems, int size, bool exact_match,
            int (*compare)(char *, char *))
{
	int high;
	int low;
	int mid;
	int comp;
	size_t offset;

	if (!table || !tag || !compare || nelems <= 0 || size <= 0)
		return(-1);
	if ((size_t)nelems > SIZE_MAX/(size_t)size)
		return(-1);
	high = nelems;
	low = 0;

	while( low<high ) {
		mid = low+(high-low)/2;
		offset = (size_t)mid*(size_t)size;

		comp = (*compare)(tag,table+offset);

		if( comp < 0 )
			high = mid;
		else if ( comp > 0 )
			low = mid + 1;
		else
			return(mid);
	}

	if( exact_match ) return(-1);
	return(low > 0 ? low-1 : 0);
}
