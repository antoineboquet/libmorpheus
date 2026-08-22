/*
 * gregory crane
 *
 * harvard university
 */
 
#include <greek.h>

#include "hasdiaer.proto.h"


int has_diaeresis(char *s)
 {
	if (!s) return(0);
 	while(*s) {
 		if( *s == DIAERESIS )
 			return(1);
 		s++;
 	}
 	return(0);
 }
