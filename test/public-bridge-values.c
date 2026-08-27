// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <string.h>

#include <greek.h>
#include <dialect.h>
#include <morphflags.h>
#include <stemtype.h>

#include "../src/bridge/legacy_values.h"

static void set_legacy_flag(unsigned char *flags, int number)
{
  const size_t index=(size_t)(number-1)/8u;
  const unsigned int bit=(unsigned int)(number-1)%8u;
  flags[index]|=(unsigned char)(UINT8_C(1)<<bit);
}

static int public_flag_is_set(const uint8_t *flags, morpheus_morph_flag index)
{
  return((flags[index/8u] & (uint8_t)(UINT8_C(1)<<(index%8u))) != 0);
}

int main(void)
{
  unsigned char legacy_flags[MORPHFLAG_STORAGE_BYTES];
  uint8_t public_flags[MORPHEUS_MORPH_FLAG_CAPACITY];

  assert(morpheus_public_part_of_speech(NOUNSTEM)==
      MORPHEUS_PART_OF_SPEECH_NOUN);
  assert(morpheus_public_part_of_speech(ADJSTEM)==
      MORPHEUS_PART_OF_SPEECH_ADJECTIVE);
  assert(morpheus_public_part_of_speech(PPARTMASK)==
      MORPHEUS_PART_OF_SPEECH_VERB);
  assert(morpheus_public_person(PERS3)==MORPHEUS_PERSON_THIRD);
  assert(morpheus_public_number(PLURAL)==MORPHEUS_NUMBER_PLURAL);
  assert(morpheus_public_tense(IMPERF)==MORPHEUS_TENSE_IMPERFECT);
  assert(morpheus_public_mood(INDICATIVE)==MORPHEUS_MOOD_INDICATIVE);
  assert(morpheus_public_dialect(IONIC|HOMERIC)==
      (MORPHEUS_DIALECT_IONIC|MORPHEUS_DIALECT_HOMERIC));
  assert(morpheus_legacy_dialect(
      MORPHEUS_DIALECT_IONIC|MORPHEUS_DIALECT_HOMERIC)==
      (IONIC|HOMERIC));
  assert(morpheus_public_region(PHOCIS|BOEOTIA)==
      (MORPHEUS_REGION_PHOCIS|MORPHEUS_REGION_BOEOTIA));

  memset(legacy_flags,0,sizeof legacy_flags);
  set_legacy_flag(legacy_flags,SYLL_AUGMENT);
  set_legacy_flag(legacy_flags,GROUP_NAME);
  morpheus_public_morph_flags(public_flags,legacy_flags);
  assert(public_flag_is_set(public_flags,MORPHEUS_MORPH_FLAG_SYLL_AUGMENT));
  assert(public_flag_is_set(public_flags,MORPHEUS_MORPH_FLAG_GROUP_NAME));
  assert(!public_flag_is_set(public_flags,MORPHEUS_MORPH_FLAG_POETIC));
  return(0);
}
