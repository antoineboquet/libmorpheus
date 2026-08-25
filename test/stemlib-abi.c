// SPDX-License-Identifier: AGPL-3.0-or-later

#include <limits.h>
#include <stdint.h>

#include <greek.h>
#include <stemtype.h>
#include <derivtype.h>
#include <dialect.h>
#include <endtags.h>

_Static_assert(CHAR_BIT == 8, "the stemlib format requires 8-bit bytes");
_Static_assert(sizeof(int32) == 4, "int32 must remain a 32-bit disk word");
_Static_assert(
	sizeof(morpheus_stemlib_offset) == 4,
	"stemlib index offsets must remain 32-bit"
);
_Static_assert(sizeof(Stemtype) == 4, "Stemtype must remain one disk word");
_Static_assert(sizeof(Derivtype) == 4, "Derivtype must remain one disk word");
_Static_assert(sizeof(Dialect) == 2, "Dialect must remain one disk short");
_Static_assert(sizeof(GeogRegion) == 4, "GeogRegion must remain one disk word");

int main(void)
{
	word_form form = {0};

	set_voice(form, 5);
	set_mood(form, 9);
	set_tense(form, 10);
	set_person(form, 5);
	set_number(form, 6);
	set_case(form, 42);
	set_degree(form, 2);
	set_gender(form, 9);
	/* Disk serialization packs these fields explicitly and does not depend on
	 * the compiler's in-memory bit-field order. Verify only representability. */
	return voice_of(form) == 5 && mood_of(form) == 9 &&
	       tense_of(form) == 10 && person_of(form) == 5 &&
	       number_of(form) == 6 && case_of(form) == 42 &&
	       degree_of(form) == 2 && gender_of(form) == 9 ? 0 : 1;
}
