#include "morphlib_internal.h"
#include <gkstring.h>

#include "ultform.proto.h"

/*
 * check to see if this form takes an accent on its ending
 * e.g. E(lla/s, E(lla/dos
 */

int ultima_form(gk_string *gstr)
{
	if (!gstr) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}
	return(Is_ultima_accent(morphflags_of(gstr)));
}
