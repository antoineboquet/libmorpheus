#ifndef MORPHEUS_GKDICT_INTERNAL_H
#define MORPHEUS_GKDICT_INTERNAL_H

#include <endtags.h>
#include <gkdict.h>
#include <gkstring.h>

#include "compnoun.proto.h"
#include "derivio.proto.h"
#include "dictio.proto.h"

#include "../gkends/retrends.proto.h"
#include "../greeklib/Fclose.proto.h"
#include "../greeklib/hasdiaer.proto.h"
#include "../greeklib/isblank.proto.h"
#include "../greeklib/stripacc.proto.h"
#include "../greeklib/stripdiaer.proto.h"
#include "../greeklib/stripmeta.proto.h"
#include "../greeklib/stripquant.proto.h"
#include "../greeklib/xstrings.proto.h"
#include "../morphlib/augment.proto.h"
#include "../morphlib/errormess.proto.h"
#include "../morphlib/gkstring.proto.h"
#include "../morphlib/morphflags.proto.h"
#include "../morphlib/morphpath.proto.h"
#include "../morphlib/morphstrcmp.proto.h"
#include "../morphlib/nextkey.proto.h"
#include "../morphlib/preverb.proto.h"
#include "../morphlib/retrentry.proto.h"
#include "../morphlib/setlang.proto.h"
#include "../morphlib/trimwhite.proto.h"

/* Implemented by the analyzer and consumed by compound-noun lookup. */
int comstemtypes(char *stem, char *stem_keys, char *ending_keys);

/* Runtime ending-index lookups consumed by dictionary analysis. */
int chckdvend(char *ending, char *keys);
int chckvstem(char *stem, char *keys);

#endif
