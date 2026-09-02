# SPDX-License-Identifier: AGPL-3.0-or-later

# Historical standalone programs retained as source reference only.
#
# These files are deliberately absent from the CMake target graph and the
# installed package.  Keep this list exhaustive: historical-utility-scope
# rejects an unclassified main() below src/.

set(MORPHEUS_SUPPORTED_ENTRY_POINTS
  src/anal/stdiomorph.c
)

set(MORPHEUS_INTERNAL_DATA_ENTRY_POINTS
  src/gkends/expendmain.c
  src/gkends/expsuffmain.c
  src/gkends/expwordmain.c
  src/gkends/imain.c
  src/gkends/smain.c
)

set(MORPHEUS_HISTORICAL_ANALYSIS_ENTRY_POINTS
  src/anal/MorphWind.c
  src/anal/deverb.c
  src/anal/digmain.c
  src/anal/findbase.c
  src/anal/lcnt.c
  src/anal/multstdiomorph.c
  src/anal/np_scan.c
  src/anal/proclems.c
  src/anal/propname.c
)

set(MORPHEUS_HISTORICAL_DATA_ENTRY_POINTS
  src/gener/conjmain.c
  src/gener/genermain.c
  src/gener/gensimpmain.c
  src/gener/genstmain.c
  src/gener/stypemain.c
  src/gkdict/indcomps.c
  src/gkdict/index.dict.c
  src/gkdict/indexnoms.main.c
  src/gkdict/indexvbs.main.c
  src/gkdict/newlats.c
  src/gkdict/newlems.c
  src/gkdict/newlems2.c
  src/gkdict/x.c
  src/gkends/x.c
)

set(MORPHEUS_HISTORICAL_PLATFORM_ENTRY_POINTS
  src/auto/beta_smk.c
  src/auto/beta_troff.c
  src/auto/qtest.c
  src/auto/qtestc.c
  src/auto/rstest.c
  src/auto/stest.c
  src/auto/test.c
  src/auto/ttest.c
  src/auto/ttestf.c
  src/auto/ttestm.c
  src/auto/ttestp.c
  src/auto/y.c
  src/play/npscan.c
  src/retr/getfld.c
  src/retr/refmain.c
  src/retr/selmain.c
  src/retr/spellmain.c
  src/retr/t.c
  src/retr/testmark.c
  src/retr/tlgscan.c
  src/retr/x.c
  src/scan/scando.c
  src/tlg/tlg_wsplit.c
)

set(MORPHEUS_HISTORICAL_DIAGNOSTIC_ENTRY_POINTS
  src/greeklib/io.c
  src/greeklib/x.c
  src/morphlib/betasort.c
  src/morphlib/testmain.c
  src/morphlib/tmain.c
)

set(MORPHEUS_HISTORICAL_ENTRY_POINTS
  ${MORPHEUS_HISTORICAL_ANALYSIS_ENTRY_POINTS}
  ${MORPHEUS_HISTORICAL_DATA_ENTRY_POINTS}
  ${MORPHEUS_HISTORICAL_PLATFORM_ENTRY_POINTS}
  ${MORPHEUS_HISTORICAL_DIAGNOSTIC_ENTRY_POINTS}
)
