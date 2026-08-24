#include "morphlib_internal.h"
#include <gkstring.h>

#include "penultform.proto.h"

int penult_form(gk_string *gstring, word_form form_info)
{
	Stemtype stemtype;

	if (!gstring) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}
	stemtype = stemtype_of(gstring);

	if( case_of(form_info) == VOCATIVE && 
		number_of(form_info) == SINGULAR &&
		(stemtype & DECL3) &&
		Is_pers_name(morphflags_of(gstring)) ) {
			return(0);
	}

	if( Is_penult_accent(morphflags_of(gstring)) ) {
		return(1);
	}

	/* special case: certain infinitives with acc on penult, not antep. */
	if ( (voice_of(form_info) == ACTIVE && 
			mood_of(form_info) == PARTICIPLE &&
			gender_of(form_info) == NEUTER &&
			number_of(form_info) == SINGULAR &&
			(case_of(form_info) == NOMINATIVE || case_of(form_info)==ACCUSATIVE))
			/* neuter active participles */
	    || (voice_of(form_info) & MEDIO_PASS && 
			mood_of(form_info) == INFINITIVE && 
			tense_of(form_info) == PERFECT ) )
			/* perfect m/p inf */ 
				return(1);
	return(0);
}
