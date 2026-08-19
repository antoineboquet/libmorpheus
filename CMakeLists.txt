cmake_minimum_required(VERSION 3.25)

project(
  libmorpheus
  VERSION 0.1.0
  DESCRIPTION "Modernization of the Morpheus morphological analyzer"
  LANGUAGES C
)

include(CTest)
include(GNUInstallDirs)

option(MORPHEUS_BUILD_CRUNCHER "Build the legacy-compatible cruncher CLI" ON)

set(
  MORPHEUS_STEMLIB_DIR
  "${PROJECT_SOURCE_DIR}/vendor/alpheios-morpheus/dist/stemlib"
  CACHE PATH
  "Path containing the precompiled Greek stemlib"
)

# The runtime is built as C17. GNU/Clang extensions remain enabled temporarily
# while the inherited K&R-style definitions and missing prototypes are migrated
# incrementally under the behavioral fixture suites.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(morpheus_build_options INTERFACE)
target_include_directories(
  morpheus_build_options
  INTERFACE
    "${PROJECT_SOURCE_DIR}/src/includes"
)
target_compile_options(
  morpheus_build_options
  INTERFACE
    # GCC 10 made this the default; keeping it explicit prevents a regression
    # to the historical common-symbol behaviour on older toolchains.
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fno-common>
)

add_library(morpheus_greeklib STATIC
  src/greeklib/Fclose.c
  src/greeklib/addaccent.c
  src/greeklib/addbreath.c
  src/greeklib/aspirate.c
  src/greeklib/beta_tolower.c
  src/greeklib/binlook.c
  src/greeklib/checkaccent.c
  src/greeklib/cinsert.c
  src/greeklib/do_dissim.c
  src/greeklib/endsinstr.c
  src/greeklib/getaccent.c
  src/greeklib/getaccp.c
  src/greeklib/getbreath.c
  src/greeklib/getquantity.c
  src/greeklib/getsyll.c
  src/greeklib/gkstrlen.c
  src/greeklib/hasaccent.c
  src/greeklib/hasdiaer.c
  src/greeklib/hasquant.c
  src/greeklib/isblank.c
  src/greeklib/isdiphth.c
  src/greeklib/issubstring.c
  src/greeklib/keyio.c
  src/greeklib/longbyposition.c
  src/greeklib/naccents.c
  src/greeklib/normucase.c
  src/greeklib/nsylls.c
  src/greeklib/quantprim.c
  src/greeklib/shortanalog.c
  src/greeklib/sprntGkflags.c
  src/greeklib/standalpha.c
  src/greeklib/standword.c
  src/greeklib/stripacc.c
  src/greeklib/stripacute.c
  src/greeklib/stripbreath.c
  src/greeklib/stripchar.c
  src/greeklib/stripdiaer.c
  src/greeklib/stripmeta.c
  src/greeklib/stripquant.c
  src/greeklib/stripstemsep.c
  src/greeklib/stripzeroend.c
  src/greeklib/strsqz.c
  src/greeklib/subchar.c
  src/greeklib/vaxwords.c
  src/greeklib/xstrings.c
  src/greeklib/zap2ndbreath.c
)
target_link_libraries(morpheus_greeklib PUBLIC morpheus_build_options)

add_library(morpheus_morphlib STATIC
  src/morphlib/adddomain.c
  src/morphlib/addninfix.c
  src/morphlib/antepenform.c
  src/morphlib/augment.c
  src/morphlib/beta2rtf.c
  src/morphlib/beta2smarta.c
  src/morphlib/cmpend.c
  src/morphlib/conjstem.c
  src/morphlib/endio.c
  src/morphlib/errormess.c
  src/morphlib/fixacc.c
  src/morphlib/gkstring.c
  src/morphlib/gktoasc.c
  src/morphlib/indkeys.c
  src/morphlib/is_thirdmono.c
  src/morphlib/loadeuph.c
  src/morphlib/markstem.c
  src/morphlib/morphflags.c
  src/morphlib/morphkeys.c
  src/morphlib/morphpath.c
  src/morphlib/morphstrcmp.c
  src/morphlib/new_val.c
  src/morphlib/nextkey.c
  src/morphlib/numovable.c
  src/morphlib/penultform.c
  src/morphlib/pres_redup.c
  src/morphlib/preverb.c
  src/morphlib/preverb2.c
  src/morphlib/preverb3.c
  src/morphlib/retrentry.c
  src/morphlib/setlang.c
  src/morphlib/smk2beta.c
  src/morphlib/sprntGkflags.c
  src/morphlib/standphon.c
  src/morphlib/trimwhite.c
  src/morphlib/ultform.c
  src/morphlib/ulttakescirc.c
)
target_link_libraries(
  morpheus_morphlib
  PUBLIC morpheus_build_options
  PRIVATE morpheus_greeklib
)

add_library(morpheus_gkends STATIC
  src/gkends/acccompos.c
  src/gkends/checkforbreath.c
  src/gkends/contract.c
  src/gkends/countendtables.c
  src/gkends/endindex.c
  src/gkends/euphend.c
  src/gkends/expendtable.c
  src/gkends/fixeta.c
  src/gkends/getcurrend.c
  src/gkends/indexendtables.c
  src/gkends/lcontr.c
  src/gkends/merge.c
  src/gkends/mkend.c
  src/gkends/nextsufftab.c
  src/gkends/retrends.c
  src/gkends/stor.c
)
target_link_libraries(
  morpheus_gkends
  PUBLIC morpheus_build_options
  PRIVATE morpheus_morphlib morpheus_greeklib
)

add_library(morpheus_gkdict STATIC
  src/gkdict/compnoun.c
  src/gkdict/derivio.c
  src/gkdict/dictio.c
)
target_link_libraries(
  morpheus_gkdict
  PUBLIC morpheus_build_options
  PRIVATE morpheus_gkends morpheus_morphlib morpheus_greeklib
)

add_library(morpheus_gener STATIC src/gener/genwd.c)
target_link_libraries(
  morpheus_gener
  PUBLIC morpheus_build_options
  PRIVATE morpheus_gkends morpheus_morphlib morpheus_greeklib
)

add_library(morpheus_anal STATIC
  src/anal/checkcrasis.c
  src/anal/checkdict.c
  src/anal/checkgenwds.c
  src/anal/checkhalf1.c
  src/anal/checkindecl.c
  src/anal/checkirreg.c
  src/anal/checknom.c
  src/anal/checkpreverb.c
  src/anal/checkstem.c
  src/anal/checkstring.c
  src/anal/checkverb.c
  src/anal/checkword.c
  src/anal/dictstems.c
  src/anal/prntanal.c
  src/anal/prvb.c
)
target_link_libraries(
  morpheus_anal
  PUBLIC morpheus_build_options
  PRIVATE
    morpheus_gener
    morpheus_gkends
    morpheus_gkdict
    morpheus_morphlib
    morpheus_greeklib
)

if(MORPHEUS_BUILD_CRUNCHER)
  add_executable(cruncher src/anal/stdiomorph.c)
  target_compile_options(
    cruncher
    PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-function-declaration>
  )
  target_link_libraries(
    cruncher
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )

  install(TARGETS cruncher RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

  if(BUILD_TESTING)
    find_package(Ruby 3.0 COMPONENTS Interpreter)
    if(Ruby_FOUND)
      add_test(
        NAME legacy_fixtures
        COMMAND
          "${CMAKE_COMMAND}" -E env
          "MORPHLIB=${PROJECT_SOURCE_DIR}/stemlib"
          "MORPHEUS_FIXTURES=${PROJECT_SOURCE_DIR}/test/fixture.json"
          "MORPHEUS_CRUNCHER=$<TARGET_FILE:cruncher>"
          "${Ruby_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/test/test.rb"
      )
      set_tests_properties(
        legacy_fixtures
        PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      )

      if(EXISTS "${MORPHEUS_STEMLIB_DIR}/Greek/steminds/nomind")
        add_test(
          NAME alpheios_greek_fixtures
          COMMAND
            "${CMAKE_COMMAND}" -E env
            "MORPHLIB=${MORPHEUS_STEMLIB_DIR}"
            "MORPHEUS_FIXTURES=${PROJECT_SOURCE_DIR}/test/alpheios-fixture.json"
            "MORPHEUS_CRUNCHER=$<TARGET_FILE:cruncher>"
            "${Ruby_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/test/test.rb"
        )
        set_tests_properties(
          alpheios_greek_fixtures
          PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        )
      else()
        message(
          WARNING
          "The Alpheios stemlib was not found at ${MORPHEUS_STEMLIB_DIR}; "
          "the Alpheios Greek fixture test will be skipped."
        )
      endif()
    endif()
  endif()
endif()
