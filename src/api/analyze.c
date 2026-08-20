#include "api_internal.h"
#include <stdio.h>
#include <string.h>
#include "../anal/anal_internal.h"
#include "../morphlib/runtime_context.h"

static void copy_text(char *destination, size_t capacity, const char *source)
{
  snprintf(destination,capacity,"%s",source);
}

static void copy_analysis(morpheus_analysis *destination, const gk_analysis *source)
{
  memset(destination,0,sizeof *destination);
  destination->struct_size=sizeof *destination;
  if(Is_verbform(source)) destination->part_of_speech=MORPHEUS_PART_OF_SPEECH_VERB;
  else if(Is_adjform(source)) destination->part_of_speech=MORPHEUS_PART_OF_SPEECH_ADJECTIVE;
  else if(Is_nounform(source)) destination->part_of_speech=MORPHEUS_PART_OF_SPEECH_NOUN;
  destination->stem_type=(uint32_t)stemtype_of(source);
  destination->derivation_type=(uint32_t)derivtype_of(source);
  destination->dialect=(uint32_t)dialect_of(source);
  destination->geographic_region=(uint32_t)geogregion_of(source);
  destination->person=(uint32_t)person_of(forminfo_of(source));
  destination->number=(uint32_t)number_of(forminfo_of(source));
  destination->gender=(uint32_t)gender_of(forminfo_of(source));
  destination->grammatical_case=(uint32_t)case_of(forminfo_of(source));
  destination->tense=(uint32_t)tense_of(forminfo_of(source));
  destination->mood=(uint32_t)mood_of(forminfo_of(source));
  destination->voice=(uint32_t)voice_of(forminfo_of(source));
  destination->degree=(uint32_t)degree_of(forminfo_of(source));
  copy_text(destination->raw,sizeof destination->raw,rawword_of(source));
  copy_text(destination->workword,sizeof destination->workword,workword_of(source));
  copy_text(destination->lemma,sizeof destination->lemma,lemma_of(source));
  copy_text(destination->preverb,sizeof destination->preverb,preverb_of(source));
  copy_text(destination->augment,sizeof destination->augment,aug1_of(source));
  copy_text(destination->stem,sizeof destination->stem,stem_of(source));
  copy_text(destination->suffix,sizeof destination->suffix,suffix_of(source));
  copy_text(destination->ending,sizeof destination->ending,endstring_of(source));
  copy_text(destination->crasis,sizeof destination->crasis,crasis_of(source));
  copy_text(destination->dictionary_form,sizeof destination->dictionary_form,dictform_of(source));
  copy_text(destination->english_form,sizeof destination->english_form,source->st_engform);
  copy_text(destination->raw_preverb,sizeof destination->raw_preverb,rawprvb_of(source));
  copy_text(destination->domains,sizeof destination->domains,domains_of(source));
  memcpy(destination->morph_flags,morphflags_of(source),sizeof destination->morph_flags);
}

morpheus_status morpheus_analyze(morpheus_context *context, const uint8_t *beta_code, size_t length, morpheus_options options, morpheus_result **result)
{
  const morpheus_options known=MORPHEUS_OPTION_STRICT_CASE|MORPHEUS_OPTION_IGNORE_ACCENTS|MORPHEUS_OPTION_VERBS_ONLY;
  morpheus_runtime_context *previous;
  morpheus_result *owned;
  gk_word *word;
  PrntFlags flags=0;
  char input[MAXWORDSIZE];
  size_t i;
  if(!context || !beta_code || !result) return(MORPHEUS_INVALID_ARGUMENT);
  *result=NULL;
  if(options & ~known) return(MORPHEUS_INVALID_ARGUMENT);
  if(length >= sizeof input) return(MORPHEUS_INPUT_TOO_LONG);
  if(memchr(beta_code,0,length)) return(MORPHEUS_INVALID_ARGUMENT);
  memcpy(input,beta_code,length);
  input[length]=0;
  if(options & MORPHEUS_OPTION_STRICT_CASE) flags|=STRICT_CASE;
  if(options & MORPHEUS_OPTION_IGNORE_ACCENTS) flags|=IGNORE_ACCENTS;
  if(options & MORPHEUS_OPTION_VERBS_ONLY) flags|=VERBS_ONLY;
  previous=morpheus_runtime_context_activate(context);
  word=morpheus_check_word(input,flags);
  if(!word) {
    morpheus_runtime_context_activate(previous);
    return(MORPHEUS_INVALID_ARGUMENT);
  }
  SortAnals(analysis_of(word),totanal_of(word));
  owned=morpheus_result_create((size_t)totanal_of(word));
  if(!owned) {
    FreeGkword(word);
    morpheus_runtime_context_activate(previous);
    return(MORPHEUS_NO_MEMORY);
  }
  for(i=0;i<owned->count;i++) copy_analysis(&owned->analyses[i],analysis_of(word)+i);
  FreeGkword(word);
  morpheus_runtime_context_activate(previous);
  *result=owned;
  return(MORPHEUS_OK);
}
