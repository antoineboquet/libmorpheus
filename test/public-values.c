#include <morpheus/morpheus.h>

#include <greek.h>
#include <dialect.h>
#include <morphflags.h>

_Static_assert(sizeof(morpheus_analysis) == 852,"ABI version 2 analysis size");
_Static_assert(MORPHEUS_ABI_VERSION == 2,"ABI version");
_Static_assert(MORPHEUS_PERSON_THIRD == 3,"normalized person code");
_Static_assert(MORPHEUS_PERSON_THIRD != PERS3,"person is not a legacy mask");
_Static_assert(MORPHEUS_NUMBER_PLURAL == 3,"normalized number code");
_Static_assert(MORPHEUS_NUMBER_PLURAL != PLURAL,"number is not a legacy mask");
_Static_assert(MORPHEUS_GENDER_MASCULINE != MASCULINE,
               "gender mask is independently ordered");
_Static_assert(MORPHEUS_CASE_NOMINATIVE != NOMINATIVE,
               "case mask is independently ordered");
_Static_assert(MORPHEUS_TENSE_IMPERFECT != IMPERF,
               "tense is not a legacy code");
_Static_assert(MORPHEUS_MOOD_INDICATIVE != INDICATIVE,
               "mood is independently ordered");
_Static_assert(MORPHEUS_VOICE_MIDDLE != MIDDLE,
               "voice mask is independently ordered");
_Static_assert(MORPHEUS_DEGREE_POSITIVE != POSITIVE,
               "absence and positive degree are distinct");
_Static_assert(MORPHEUS_DIALECT_IONIC != IONIC,
               "dialect mask is independently ordered");
_Static_assert(MORPHEUS_REGION_PHOCIS != PHOCIS,
               "region mask is independently ordered");
_Static_assert(MORPHEUS_MORPH_FLAG_SYLL_AUGMENT != SYLL_AUGMENT,
               "trait indices do not expose legacy flag numbers");
_Static_assert(MORPHEUS_MORPH_FLAG_GROUP_NAME != GROUP_NAME,
               "sparse legacy flag numbers are not exposed");
_Static_assert(MORPHEUS_MORPH_FLAG_COUNT <=
               MORPHEUS_MORPH_FLAG_CAPACITY*8u,
               "public trait capacity covers all traits");

int main(void)
{
  return(0);
}
