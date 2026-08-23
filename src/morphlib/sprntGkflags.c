#include "morphlib_internal.h"
#include	<gkstring.h>

#include "sprntGkflags.h"

/*
  Created:	04.24.92
  Author:		jjake
  This is essentially the SprintGKFlags routine, excpet I have modified it to accept
  a second delimiter for the lists of Dialects,regions,domains and morph names.
	
*/
int JakeSprintGkFlags(gk_string *gstr, char *buf, size_t capacity,
                      const char *dels, const char *more_dels, int pretty)
{
  if (!more_dels) {
    morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
    return(0);
  }
  return(SprintGkFlags(gstr,buf,capacity,dels,pretty));
}

static int append_greg_field(char *buf, size_t capacity,
                             const char *delimiter, const char *value)
{
  if ((*value || *delimiter == '\t') &&
      !Xstrncat(buf,delimiter,capacity)) return(0);
  return(!*value || Xstrncat(buf,value,capacity));
}

int GregSprintGkFlags(gk_string *gstr, char *buf, size_t capacity,
                      const char *dels, const char *more_dels, int pretty)
{
  char dialbuf[LONGSTRING*2];
  word_form wf;
  size_t initial = 0;

  if (!gstr || !buf || !capacity || !dels || !more_dels) goto failed;
  while (initial < capacity && buf[initial]) initial++;
  if (initial == capacity) goto failed;
  wf = forminfo_of(gstr);
  if (!append_greg_field(buf,capacity,dels,NameOfTense(wf)) ||
      !append_greg_field(buf,capacity,dels,NameOfMood(wf)) ||
      !append_greg_field(buf,capacity,dels,NameOfVoice(wf)) ||
      !append_greg_field(buf,capacity,dels,NameOfGender(wf)) ||
      !append_greg_field(buf,capacity,dels,NameOfCase(wf)) ||
      !append_greg_field(buf,capacity,dels,NameOfDegree(wf)) ||
      !append_greg_field(buf,capacity,dels,NameOfPerson(wf)) ||
      !append_greg_field(buf,capacity,dels,NameOfNumber(wf))) goto rollback;

  if (!DialectNames(dialect_of(gstr),dialbuf,sizeof dialbuf,more_dels) ||
      !Xstrncat(buf,"\t",capacity) || !Xstrncat(buf,dialbuf,capacity) ||
      !GeogRegionNames(geogregion_of(gstr),dialbuf,sizeof dialbuf,more_dels) ||
      !append_greg_field(buf,capacity,dels,dialbuf) ||
      !DomainNames(domains_of(gstr),dialbuf,sizeof dialbuf,more_dels) ||
      !append_greg_field(buf,capacity,dels,dialbuf) ||
      !MorphNames(morphflags_of(gstr),dialbuf,sizeof dialbuf,more_dels,pretty) ||
      !Xstrncat(buf,"\t",capacity) || !Xstrncat(buf,dialbuf,capacity) ||
      !Xstrncat(buf,"\t",capacity) ||
      !Xstrncat(buf,NameOfStemtype(stemtype_of(gstr)),capacity)) goto rollback;
  if (*NameOfDerivtype(derivtype_of(gstr)) &&
      (!Xstrncat(buf,",",capacity) ||
       !Xstrncat(buf,NameOfDerivtype(derivtype_of(gstr)),capacity)))
    goto rollback;
  return(1);
rollback:
  buf[initial] = 0;
failed:
  morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
  return(0);
}
