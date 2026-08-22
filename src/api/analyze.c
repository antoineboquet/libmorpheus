#include "api_internal.h"
#include <string.h>
#include "../anal/anal_internal.h"
#include "../morphlib/runtime_context_internal.h"

typedef struct {
  int crasis_disabled;
  int quick_enabled;
  int hq_dictionary;
  Dialect wanted_dialects;
  int wanted_dialects_initialized;
} request_state;

static void apply_request_options(
    morpheus_context *context, morpheus_options options, request_state *saved)
{
  saved->crasis_disabled=context->analysis_crasis_disabled;
  saved->quick_enabled=context->analysis_quick_enabled;
  saved->hq_dictionary=context->dictionary_hq_mode;
  saved->wanted_dialects=context->analysis_wanted_dialects;
  saved->wanted_dialects_initialized=
      context->analysis_wanted_dialects_initialized;
  context->analysis_crasis_disabled=!!(options & MORPHEUS_OPTION_NO_CRASIS);
  context->analysis_quick_enabled=!!(options & MORPHEUS_OPTION_QUICK);
  context->dictionary_hq_mode=!!(options & MORPHEUS_OPTION_HQ_DICTIONARY);
  context->analysis_wanted_dialects=(Dialect)(
      (options & MORPHEUS_OPTION_DIALECT_MASK) >>
      MORPHEUS_OPTION_DIALECT_SHIFT);
  context->analysis_wanted_dialects_initialized=1;
}

static void restore_request_state(
    morpheus_context *context, const request_state *saved)
{
  context->analysis_crasis_disabled=saved->crasis_disabled;
  context->analysis_quick_enabled=saved->quick_enabled;
  context->dictionary_hq_mode=saved->hq_dictionary;
  context->analysis_wanted_dialects=saved->wanted_dialects;
  context->analysis_wanted_dialects_initialized=
      saved->wanted_dialects_initialized;
}

static int copy_text(char *destination, size_t capacity, const char *source)
{
  const size_t length=strlen(source);
  const size_t copied=length < capacity ? length : capacity-1;
  memcpy(destination,source,copied);
  destination[copied]=0;
  return(length >= capacity);
}

static morpheus_truncated_fields copy_analysis(
    morpheus_analysis *destination, const gk_analysis *source)
{
  morpheus_truncated_fields truncated=0;
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
  if(copy_text(destination->raw,sizeof destination->raw,rawword_of(source)))
    truncated|=MORPHEUS_TRUNCATED_RAW;
  if(copy_text(destination->workword,sizeof destination->workword,
               workword_of(source)))
    truncated|=MORPHEUS_TRUNCATED_WORKWORD;
  if(copy_text(destination->lemma,sizeof destination->lemma,lemma_of(source)))
    truncated|=MORPHEUS_TRUNCATED_LEMMA;
  if(copy_text(destination->preverb,sizeof destination->preverb,
               preverb_of(source)))
    truncated|=MORPHEUS_TRUNCATED_PREVERB;
  if(copy_text(destination->augment,sizeof destination->augment,aug1_of(source)))
    truncated|=MORPHEUS_TRUNCATED_AUGMENT;
  if(copy_text(destination->stem,sizeof destination->stem,stem_of(source)))
    truncated|=MORPHEUS_TRUNCATED_STEM;
  if(copy_text(destination->suffix,sizeof destination->suffix,suffix_of(source)))
    truncated|=MORPHEUS_TRUNCATED_SUFFIX;
  if(copy_text(destination->ending,sizeof destination->ending,
               endstring_of(source)))
    truncated|=MORPHEUS_TRUNCATED_ENDING;
  if(copy_text(destination->crasis,sizeof destination->crasis,crasis_of(source)))
    truncated|=MORPHEUS_TRUNCATED_CRASIS;
  if(copy_text(destination->dictionary_form,sizeof destination->dictionary_form,
               dictform_of(source)))
    truncated|=MORPHEUS_TRUNCATED_DICTIONARY_FORM;
  if(copy_text(destination->english_form,sizeof destination->english_form,
               source->st_engform))
    truncated|=MORPHEUS_TRUNCATED_ENGLISH_FORM;
  if(copy_text(destination->raw_preverb,sizeof destination->raw_preverb,
               rawprvb_of(source)))
    truncated|=MORPHEUS_TRUNCATED_RAW_PREVERB;
  if(copy_text(destination->domains,sizeof destination->domains,
               domains_of(source)))
    truncated|=MORPHEUS_TRUNCATED_DOMAINS;
  memcpy(destination->morph_flags,morphflags_of(source),sizeof destination->morph_flags);
  return(truncated);
}

morpheus_status morpheus_analyze(morpheus_context *context, const uint8_t *beta_code, size_t length, morpheus_options options, morpheus_result **result)
{
  const morpheus_options known=
      MORPHEUS_OPTION_STRICT_CASE|MORPHEUS_OPTION_IGNORE_ACCENTS|
      MORPHEUS_OPTION_VERBS_ONLY|MORPHEUS_OPTION_NO_CRASIS|
      MORPHEUS_OPTION_QUICK|MORPHEUS_OPTION_HQ_DICTIONARY|
      MORPHEUS_OPTION_DIALECT_MASK;
  morpheus_runtime_context *previous;
  morpheus_status runtime_status;
  request_state saved;
  morpheus_result *owned;
  gk_word *word;
  PrntFlags flags=0;
  char input[MAXWORDSIZE];
  size_t i;
  if(!context || !beta_code || !result) return(MORPHEUS_INVALID_ARGUMENT);
  *result=NULL;
  if(options & ~known) return(MORPHEUS_INVALID_ARGUMENT);
  if((options & MORPHEUS_OPTION_DIALECT_MASK) &&
     context->language != GREEK)
    return(MORPHEUS_INVALID_ARGUMENT);
  if(length >= sizeof input) return(MORPHEUS_INPUT_TOO_LONG);
  if(memchr(beta_code,0,length)) return(MORPHEUS_INVALID_ARGUMENT);
  memcpy(input,beta_code,length);
  input[length]=0;
  if(options & MORPHEUS_OPTION_STRICT_CASE) flags|=STRICT_CASE;
  if(options & MORPHEUS_OPTION_IGNORE_ACCENTS) flags|=IGNORE_ACCENTS;
  if(options & MORPHEUS_OPTION_VERBS_ONLY) flags|=VERBS_ONLY;
  previous=morpheus_runtime_context_activate(context);
  morpheus_runtime_context_clear_error(context);
  apply_request_options(context,options,&saved);
  word=morpheus_check_word(input,flags);
  runtime_status=morpheus_runtime_status(context);
  if(runtime_status != MORPHEUS_OK) {
    if(word) FreeGkword(word);
    restore_request_state(context,&saved);
    morpheus_runtime_context_activate(previous);
    return(runtime_status);
  }
  if(!word) {
    restore_request_state(context,&saved);
    morpheus_runtime_context_activate(previous);
    return(MORPHEUS_INVALID_ARGUMENT);
  }
  SortAnals(analysis_of(word),totanal_of(word));
  owned=morpheus_result_create((size_t)totanal_of(word));
  if(!owned) {
    FreeGkword(word);
    restore_request_state(context,&saved);
    morpheus_runtime_context_activate(previous);
    return(MORPHEUS_NO_MEMORY);
  }
  for(i=0;i<owned->count;i++) {
    owned->truncated_fields[i]=copy_analysis(
        &owned->analyses[i],analysis_of(word)+i);
    memcpy(owned->all_morph_flags+i*MORPHEUS_ALL_MORPH_FLAG_CAPACITY,
        morphflags_of(analysis_of(word)+i),
        MORPHEUS_ALL_MORPH_FLAG_CAPACITY);
  }
  FreeGkword(word);
  restore_request_state(context,&saved);
  morpheus_runtime_context_activate(previous);
  *result=owned;
  return(MORPHEUS_OK);
}
