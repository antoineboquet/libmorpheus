// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>

#include "../src/greeklib/longbyposition.proto.h"

int main(void)
{
	char single_consonant[] = "ak";
	char double_consonant[] = "akk";
	char double_letter[] = "az";
	char stop_liquid[] = "akr";
	char labial_liquid[] = "apr";
	char liquid_stop[] = "arp";
	char marked_vowel[] = "a)/kr";

	assert(longbyposition(single_consonant) == 0);
	assert(longbyposition(double_consonant) == 1);
	assert(longbyposition(double_letter) == 1);
	assert(longbyposition(stop_liquid) == 2);
	assert(longbyposition(labial_liquid) == 2);
	assert(longbyposition(liquid_stop) == 1);
	assert(longbyposition(marked_vowel) == 2);
	return 0;
}
