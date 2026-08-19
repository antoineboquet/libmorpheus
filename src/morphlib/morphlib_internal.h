#ifndef MORPHEUS_MORPHLIB_INTERNAL_H
#define MORPHEUS_MORPHLIB_INTERNAL_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../greeklib/greeklib_internal.h"

#include "adddomain.proto.h"
#include "addninfix.proto.h"
#include "antepenform.proto.h"
#include "augment.proto.h"
#include "beta2rtf.proto.h"
#include "beta2smarta.proto.h"
#include "cmpend.proto.h"
#include "conjstem.proto.h"
#include "endio.proto.h"
#include "errormess.proto.h"
#include "fixacc.proto.h"
#include "gkstring.proto.h"
#include "is_thirdmono.proto.h"
#include "loadeuph.proto.h"
#include "markstem.proto.h"
#include "morphflags.proto.h"
#include "morphkeys.proto.h"
#include "morphpath.proto.h"
#include "morphstrcmp.proto.h"
#include "new_val.proto.h"
#include "nextkey.proto.h"
#include "numovable.proto.h"
#include "penultform.proto.h"
#include "pres_redup.proto.h"
#include "preverb.proto.h"
#include "preverb2.proto.h"
#include "preverb3.proto.h"
#include "setlang.proto.h"
#include "smk2beta.proto.h"
#include "sprntGkflags.h"
#include "standphon.proto.h"
#include "trimwhite.proto.h"
#include "ultform.proto.h"
#include "ulttakescirc.proto.h"

void FixPersAcc2(gk_string *gstring, MorphFlags *flags, gk_string *stem,
                 char *ending, char *word, word_form form, int is_ending);
void init_betatab(void);
void localtrimwhite(char *string, int length);
void set_gkorder(char *table);

#endif
