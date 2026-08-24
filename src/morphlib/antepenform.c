#include "morphlib_internal.h"
#include <gkstring.h>

#include "antepenform.proto.h"

int antepen_form(gk_string *gstring, word_form form_info)
{
	if (!gstring) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}

	if( Is_antepen_accent(morphflags_of(gstring)) ) {
		return(1);
	}
	return(0);
}
