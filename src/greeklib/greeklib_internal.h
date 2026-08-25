/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_GREEKLIB_INTERNAL_H
#define MORPHEUS_GREEKLIB_INTERNAL_H

#include <gkstring.h>

#include "Fclose.proto.h"
#include "addaccent.proto.h"
#include "addbreath.proto.h"
#include "aspirate.proto.h"
#include "beta_tolower.proto.h"
#include "binlook.proto.h"
#include "checkaccent.proto.h"
#include "cinsert.proto.h"
#include "do_dissim.proto.h"
#include "endsinstr.proto.h"
#include "getaccent.proto.h"
#include "getaccp.proto.h"
#include "getbreath.proto.h"
#include "getquantity.proto.h"
#include "getsyll.proto.h"
#include "gkstrlen.proto.h"
#include "hasaccent.proto.h"
#include "hasdiaer.proto.h"
#include "hasquant.proto.h"
#include "isblank.proto.h"
#include "isdiphth.proto.h"
#include "issubstring.proto.h"
#include "keyio.proto.h"
#include "longbyposition.proto.h"
#include "naccents.proto.h"
#include "normucase.proto.h"
#include "nsylls.proto.h"
#include "quantprim.proto.h"
#include "shortanalog.proto.h"
#include "sprntGkflags.proto.h"
#include "standalpha.proto.h"
#include "standword.proto.h"
#include "stripacc.proto.h"
#include "stripacute.proto.h"
#include "stripbreath.proto.h"
#include "stripchar.proto.h"
#include "stripdiaer.proto.h"
#include "stripmeta.proto.h"
#include "stripquant.proto.h"
#include "stripstemsep.proto.h"
#include "stripzeroend.proto.h"
#include "strsqz.proto.h"
#include "subchar.proto.h"
#include "vaxwords.proto.h"
#include "xstrings.proto.h"
#include "zap2ndbreath.proto.h"

/* Formatting helpers are implemented by morphlib. */
int DialectNames(Dialect dialect, char *buffer, size_t capacity,
                 const char *delimiter);
int MorphNames(MorphFlags *flags, char *buffer, size_t capacity,
               const char *delimiter, int pretty);

#endif
