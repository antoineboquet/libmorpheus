#include <limits.h>
#include <stdint.h>
#include <string.h>

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
_Static_assert(sizeof(word_form) == 4, "word_form must remain one disk word");
_Static_assert(sizeof(verb_form) == 4, "verb_form must remain one disk word");
_Static_assert(sizeof(adj_form) == 4, "adj_form must remain one disk word");
_Static_assert(sizeof(Stemtype) == 4, "Stemtype must remain one disk word");
_Static_assert(sizeof(Derivtype) == 4, "Derivtype must remain one disk word");
_Static_assert(sizeof(Dialect) == 2, "Dialect must remain one disk short");
_Static_assert(sizeof(GeogRegion) == 4, "GeogRegion must remain one disk word");

int main(void)
{
	word_form form = {0};
	uint32_t packed = 0;

	set_voice(form, 5);
	set_mood(form, 9);
	set_tense(form, 10);
	set_person(form, 5);
	set_number(form, 6);
	set_case(form, 42);
	set_degree(form, 2);
	set_gender(form, 9);
	memcpy(&packed, &form, sizeof packed);

	/*
	 * The compiled stemlib stores this historical bit-field as a 32-bit
	 * little-endian word. This check rejects a compiler/ABI that allocates the
	 * fields in another order even when all scalar widths happen to match.
	 */
	return packed == UINT32_C(0x1355ad4d) ? 0 : 1;
}
