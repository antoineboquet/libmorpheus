/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_GENER_INTERNAL_H
#define MORPHEUS_GENER_INTERNAL_H

#include <gkstring.h>

#include "genwd.proto.h"

#include "../gkends/retrends.proto.h"
#include "../greeklib/isblank.proto.h"
#include "../greeklib/nsylls.proto.h"
#include "../greeklib/stripacc.proto.h"
#include "../greeklib/stripacute.proto.h"
#include "../greeklib/stripmeta.proto.h"
#include "../greeklib/stripstemsep.proto.h"
#include "../greeklib/xstrings.proto.h"
#include "../morphlib/augment.proto.h"
#include "../morphlib/errormess.proto.h"
#include "../morphlib/fixacc.proto.h"
#include "../morphlib/gkstring.proto.h"
#include "../morphlib/morphflags.proto.h"
#include "../morphlib/morphkeys.proto.h"
#include "../morphlib/nextkey.proto.h"
#include "../morphlib/preverb.proto.h"

int NextDictLine(FILE *input, char *word, char *word_keys, char *prefix);
int BuildAWord(gk_word *word, gk_string *ending, gk_word *forms);
int BuildANoun(gk_word *word, gk_string *ending, gk_word *forms);
int BuildAVerb(gk_word *word, gk_string *ending, gk_word *forms);
void MonoSyllVb(gk_word *forms, word_form form, char *preverb);

#endif
