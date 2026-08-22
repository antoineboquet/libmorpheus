#include "morphlib_internal.h"
#include <gkstring.h>	

#include "new_val.proto.h"

void
new_person(gk_string *gstr, unsigned long val)
{
		set_person(forminfo_of(gstr),(unsigned int)(val & 07UL));
}
void
new_number(gk_string *gstr, unsigned long val)
{
		set_number(forminfo_of(gstr),(unsigned int)(val & 07UL));
}

void
new_case(gk_string *gstr, unsigned long val)
{
		set_case(forminfo_of(gstr),
			case_of(forminfo_of(gstr)) | (unsigned int)(val & 077UL));
}


void
new_tense(gk_string *gstr, unsigned long val)
{
		set_tense(forminfo_of(gstr),(unsigned int)(val & 017UL));
}


void
new_voice(gk_string *gstr, unsigned long val)
{
		unsigned int voice = (unsigned int)(val & 07UL);

		if( voice == ACTIVE && (voice_of(forminfo_of(gstr)) & (MEDIO_PASS) ) ) 
			set_voice(forminfo_of(gstr),voice & 07U);
		else if( ( voice & (MEDIO_PASS) ) && (voice_of(forminfo_of(gstr)) ==ACTIVE) )
			set_voice(forminfo_of(gstr),voice & 07U);
		else set_voice(forminfo_of(gstr),
			(voice_of(forminfo_of(gstr)) | voice) & 07U);
}


void
new_mood(gk_string *gstr, unsigned long val)
{
		set_mood(forminfo_of(gstr),(unsigned int)(val & 017UL));
}



void
new_degree(gk_string *gstr, unsigned long val)
{
		set_degree(forminfo_of(gstr),(unsigned int)(val & 03UL));
}


void
new_gender(gk_string *gstr, unsigned long val)
{
		set_gender(forminfo_of(gstr),
			gender_of(forminfo_of(gstr)) | (unsigned int)(val & 017UL));
}


void
new_dialect(gk_string *gstr, unsigned long val)
{
		add_dialect(gstr,(Dialect)val);
}


void
new_region(gk_string *gstr, unsigned long val)
{
		add_geogregion(gstr,(GeogRegion)val);
}


void
new_morphflags(gk_string *gstr, unsigned long val)
{

		add_morphflag(morphflags_of(gstr),(int)val);
}
	

void
new_stemtype(gk_string *gstr, unsigned long val)
{
		set_stemtype(gstr,(Stemtype)val);
}


void
new_domain(gk_string *gstr, unsigned long val)
{
		add_domain(gstr,(int)val);
}

	

void
new_derivtype(gk_string *gstr, unsigned long val)
{
		set_derivtype(gstr,(Derivtype)val);
}
