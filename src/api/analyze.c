#include "api_internal.h"
#include <stdio.h>
#include <string.h>
#include <gkdict.h>
#include "../anal/anal_internal.h"
#include "../morphlib/runtime_context_internal.h"

typedef struct {
  int crasis_disabled;
  int quick_enabled;
  int hq_dictionary;
  Dialect wanted_dialects;
  int wanted_dialects_initialized;
} request_state;

static const char *language_directory(int language)
{
  if(language == LATIN) return("Latin");
  if(language == ITALIAN) return("Italian");
  if(language == GREEK) return("Greek");
  return(NULL);
}

static int hq_dictionary_available(morpheus_context *context)
{
  static const char * const required[]={STEMLIST,STEMLIST ".lindex"};
  const char *directory;
  size_t i;

  if(context->dictionary_hq_availability_checked &&
     context->dictionary_hq_availability_language == context->language)
    return(context->dictionary_hq_available);
  context->dictionary_hq_availability_checked=1;
  context->dictionary_hq_availability_language=context->language;
  context->dictionary_hq_available=0;
  directory=language_directory(context->language);
  if(!context->stemlib_path || !directory) return(0);
  for(i=0;i<sizeof required/sizeof required[0];i++) {
    char path[MAXPATHNAME];
    FILE *file;
    int written=snprintf(path,sizeof path,"%s/%s/%s",
                         context->stemlib_path,directory,required[i]);
    if(written < 0 || (size_t)written >= sizeof path) return(0);
    file=fopen(path,"rb");
    if(!file) return(0);
    if(fgetc(file)==EOF) {
      fclose(file);
      return(0);
    }
    fclose(file);
  }
  context->dictionary_hq_available=1;
  return(1);
}

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

static int stem_type_name_is(const char *name, const char *candidate)
{
  return(name && strcmp(name,candidate)==0);
}

static morpheus_part_of_speech classify_part_of_speech(
    const gk_analysis *source)
{
  const char *name;

  if(Is_verbform(source)) return(MORPHEUS_PART_OF_SPEECH_VERB);
  name=NameOfStemtype(stemtype_of(source));
  if(stem_type_name_is(name,"adverb"))
    return(MORPHEUS_PART_OF_SPEECH_ADVERB);
  if(stem_type_name_is(name,"article"))
    return(MORPHEUS_PART_OF_SPEECH_ARTICLE);
  if(stem_type_name_is(name,"demonstr") ||
     stem_type_name_is(name,"indef") ||
     stem_type_name_is(name,"indef_pron") ||
     stem_type_name_is(name,"indef_rel_pron") ||
     stem_type_name_is(name,"interrog") ||
     stem_type_name_is(name,"pers_pron") ||
     stem_type_name_is(name,"pron1") ||
     stem_type_name_is(name,"pron3") ||
     stem_type_name_is(name,"pron_adj1") ||
     stem_type_name_is(name,"pron_adj3") ||
     stem_type_name_is(name,"relative") ||
     stem_type_name_is(name,"rel_pron"))
    return(MORPHEUS_PART_OF_SPEECH_PRONOUN);
  if(stem_type_name_is(name,"numeral"))
    return(MORPHEUS_PART_OF_SPEECH_NUMERAL);
  if(stem_type_name_is(name,"prep"))
    return(MORPHEUS_PART_OF_SPEECH_PREPOSITION);
  if(stem_type_name_is(name,"conj") || stem_type_name_is(name,"connect"))
    return(MORPHEUS_PART_OF_SPEECH_CONJUNCTION);
  if(stem_type_name_is(name,"particle") ||
     stem_type_name_is(name,"expletive"))
    return(MORPHEUS_PART_OF_SPEECH_PARTICLE);
  if(stem_type_name_is(name,"exclam"))
    return(MORPHEUS_PART_OF_SPEECH_INTERJECTION);
  if(stem_type_name_is(name,"alphabetic") ||
     stem_type_name_is(name,"indecl_noun") ||
     stem_type_name_is(name,"irreg_decl3"))
    return(MORPHEUS_PART_OF_SPEECH_NOUN);
  if(stem_type_name_is(name,"indecl"))
    return(MORPHEUS_PART_OF_SPEECH_UNKNOWN);
  if(Is_nounform(source)) return(MORPHEUS_PART_OF_SPEECH_NOUN);
  if(Is_adjform(source)) return(MORPHEUS_PART_OF_SPEECH_ADJECTIVE);
  return(MORPHEUS_PART_OF_SPEECH_UNKNOWN);
}

static morpheus_truncated_fields copy_analysis(
    morpheus_analysis *destination, const gk_analysis *source)
{
  morpheus_truncated_fields truncated=0;
  memset(destination,0,sizeof *destination);
  destination->struct_size=sizeof *destination;
  destination->part_of_speech=classify_part_of_speech(source);
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
  if((options & MORPHEUS_OPTION_HQ_DICTIONARY) &&
     !hq_dictionary_available(context))
    return(MORPHEUS_STEMLIB_ERROR);
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
