#include <morpheus/morpheus.h>

#include <greek.h>
#include <dialect.h>
#include <morphflags.h>

_Static_assert(sizeof(morpheus_analysis) == 860,"ABI version 1 analysis size");
_Static_assert(MORPHEUS_PERSON_FIRST == PERS1,"first person code");
_Static_assert(MORPHEUS_PERSON_SECOND == PERS2,"second person code");
_Static_assert(MORPHEUS_PERSON_THIRD == PERS3,"third person code");
_Static_assert(MORPHEUS_NUMBER_SINGULAR == SINGULAR,"singular code");
_Static_assert(MORPHEUS_NUMBER_DUAL == DUAL,"dual code");
_Static_assert(MORPHEUS_NUMBER_PLURAL == PLURAL,"plural code");
_Static_assert(MORPHEUS_GENDER_MASCULINE == MASCULINE,"masculine code");
_Static_assert(MORPHEUS_GENDER_FEMININE == FEMININE,"feminine code");
_Static_assert(MORPHEUS_GENDER_NEUTER == NEUTER,"neuter code");
_Static_assert(MORPHEUS_GENDER_ADVERBIAL == ADVERBIAL,"adverbial code");
_Static_assert(MORPHEUS_CASE_NOMINATIVE == NOMINATIVE,"nominative code");
_Static_assert(MORPHEUS_CASE_GENITIVE == GENITIVE,"genitive code");
_Static_assert(MORPHEUS_CASE_DATIVE == DATIVE,"dative code");
_Static_assert(MORPHEUS_CASE_ACCUSATIVE == ACCUSATIVE,"accusative code");
_Static_assert(MORPHEUS_CASE_VOCATIVE == VOCATIVE,"vocative code");
_Static_assert(MORPHEUS_CASE_ABLATIVE == ABLATIVE,"ablative code");
_Static_assert(MORPHEUS_TENSE_PRESENT == PRESENT,"present code");
_Static_assert(MORPHEUS_TENSE_IMPERFECT == IMPERF,"imperfect code");
_Static_assert(MORPHEUS_TENSE_FUTURE == FUTURE,"future code");
_Static_assert(MORPHEUS_TENSE_AORIST == AORIST,"aorist code");
_Static_assert(MORPHEUS_TENSE_PERFECT == PERFECT,"perfect code");
_Static_assert(MORPHEUS_TENSE_PLUPERFECT == PLUPERF,"pluperfect code");
_Static_assert(MORPHEUS_TENSE_FUTURE_PERFECT == FUTPERF,
               "future perfect code");
_Static_assert(MORPHEUS_TENSE_PAST_ABSOLUTE == PASTABSOLUTE,
               "past absolute code");
_Static_assert(MORPHEUS_MOOD_INDICATIVE == INDICATIVE,"indicative code");
_Static_assert(MORPHEUS_MOOD_SUBJUNCTIVE == SUBJUNCTIVE,"subjunctive code");
_Static_assert(MORPHEUS_MOOD_OPTATIVE == OPTATIVE,"optative code");
_Static_assert(MORPHEUS_MOOD_IMPERATIVE == IMPERATIVE,"imperative code");
_Static_assert(MORPHEUS_MOOD_INFINITIVE == INFINITIVE,"infinitive code");
_Static_assert(MORPHEUS_MOOD_PARTICIPLE == PARTICIPLE,"participle code");
_Static_assert(MORPHEUS_MOOD_GERUNDIVE == GERUNDIVE,"gerundive code");
_Static_assert(MORPHEUS_MOOD_SUPINE == SUPINE,"supine code");
_Static_assert(MORPHEUS_MOOD_CONDITIONAL == CONDITIONAL,"conditional code");
_Static_assert(MORPHEUS_VOICE_ACTIVE == ACTIVE,"active code");
_Static_assert(MORPHEUS_VOICE_MIDDLE == MIDDLE,"middle code");
_Static_assert(MORPHEUS_VOICE_PASSIVE == PASSIVE,"passive code");
_Static_assert(MORPHEUS_DEGREE_POSITIVE == POSITIVE,"positive code");
_Static_assert(MORPHEUS_DEGREE_COMPARATIVE == COMPARATIVE,"comparative code");
_Static_assert(MORPHEUS_DEGREE_SUPERLATIVE == SUPERLATIVE,"superlative code");
_Static_assert(MORPHEUS_DIALECT_ATTIC == ATTIC,"Attic code");
_Static_assert(MORPHEUS_DIALECT_EPIC == EPIC,"epic code");
_Static_assert(MORPHEUS_DIALECT_PROSE == PROSE,"prose code");
_Static_assert(MORPHEUS_REGION_PHOCIS == PHOCIS,"Phocis code");
_Static_assert(MORPHEUS_REGION_BOEOTIA == BOEOTIA,"Boeotia code");
_Static_assert(sizeof(morpheus_morph_flag) == sizeof(uint32_t),
               "morph flag type width");
_Static_assert(MORPHEUS_MORPH_FLAG_SYLL_AUGMENT == SYLL_AUGMENT,
               "syllabic augment flag");
_Static_assert(MORPHEUS_MORPH_FLAG_SUFFIX_ACCENT == SUFF_ACC,
               "suffix accent flag");
_Static_assert(MORPHEUS_MORPH_FLAG_RHO_ETA_IOTA_ALPHA == R_E_I_ALPHA,
               "rho eta iota alpha flag");
_Static_assert(MORPHEUS_MORPH_FLAG_INTERVOCALIC_S_TO_H == INTERV_S_TO_H,
               "intervocalic sigma flag");
_Static_assert(MORPHEUS_MORPH_FLAG_PRESENT_REDUPLICATION == PRES_REDUPL,
               "present reduplication flag");
_Static_assert(MORPHEUS_MORPH_FLAG_TAU_PREVERB == T_PREVB,
               "tau preverb flag");
_Static_assert(MORPHEUS_MORPH_FLAG_GROUP_NAME == GROUP_NAME,
               "group name flag");

int main(void)
{
  return(0);
}
