cmake_minimum_required(VERSION 3.25)

project(
  libmorpheus
  VERSION 0.1.0
  DESCRIPTION "Modernization of the Morpheus morphological analyzer"
  LANGUAGES C
)

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(Threads REQUIRED)

option(MORPHEUS_BUILD_CRUNCHER "Build the legacy-compatible cruncher CLI" ON)
option(
  MORPHEUS_ENABLE_SANITIZERS
  "Build the runtime with AddressSanitizer and UndefinedBehaviorSanitizer"
  OFF
)
option(
  MORPHEUS_ENABLE_THREAD_SANITIZER
  "Build the runtime with ThreadSanitizer"
  OFF
)

if(MORPHEUS_ENABLE_SANITIZERS AND MORPHEUS_ENABLE_THREAD_SANITIZER)
  message(FATAL_ERROR "AddressSanitizer and ThreadSanitizer cannot be combined")
endif()

set(
  MORPHEUS_STEMLIB_DIR
  "${PROJECT_SOURCE_DIR}/vendor/alpheios-morpheus/dist/stemlib"
  CACHE PATH
  "Path containing the precompiled Greek stemlib"
)

# Build the runtime against the ISO C17 language surface. POSIX interfaces used
# by the compatibility layer are requested explicitly at their call sites.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
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
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wformat=2>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=format>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wpedantic>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=pedantic>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=strict-prototypes>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=old-style-definition>
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-Werror=overflow>
    $<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:-Werror=constant-conversion>
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-Werror=format-overflow>
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-Werror=format-truncation>
)

if(MORPHEUS_ENABLE_SANITIZERS)
  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      morpheus_build_options
      INTERFACE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -fno-sanitize-recover=all
    )
    target_link_options(
      morpheus_build_options
      INTERFACE
        -fsanitize=address,undefined
        -fno-sanitize-recover=all
    )
  else()
    message(
      FATAL_ERROR
      "MORPHEUS_ENABLE_SANITIZERS requires GCC, Clang, or AppleClang"
    )
  endif()
endif()

if(MORPHEUS_ENABLE_THREAD_SANITIZER)
  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      morpheus_build_options
      INTERFACE
        -fsanitize=thread
        -fno-omit-frame-pointer
        -fno-sanitize-recover=all
    )
    target_link_options(
      morpheus_build_options
      INTERFACE
        -fsanitize=thread
        -fno-sanitize-recover=all
    )
  else()
    message(
      FATAL_ERROR
      "MORPHEUS_ENABLE_THREAD_SANITIZER requires GCC or Clang"
    )
  endif()
endif()

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
target_compile_options(
  morpheus_greeklib
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-int>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=incompatible-pointer-types>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=return-type>
)

if(BUILD_TESTING)
  add_executable(morpheus_stemlib_abi_test test/stemlib-abi.c)
  target_link_libraries(
    morpheus_stemlib_abi_test PRIVATE morpheus_build_options
  )
  add_test(NAME stemlib_abi COMMAND morpheus_stemlib_abi_test)
endif()

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
target_compile_options(
  morpheus_morphlib
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-int>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=incompatible-pointer-types>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=return-type>
)

if(BUILD_TESTING)
  add_executable(morpheus_runtime_context_test test/runtime-context.c)
  target_link_libraries(
    morpheus_runtime_context_test
    PRIVATE morpheus_morphlib morpheus_build_options
  )
  add_test(NAME runtime_context COMMAND morpheus_runtime_context_test)
  add_executable(morpheus_byte_conversion_test test/byte-conversion.c)
  target_link_libraries(
    morpheus_byte_conversion_test
    PRIVATE morpheus_morphlib morpheus_build_options
  )
  add_test(NAME byte_conversion COMMAND morpheus_byte_conversion_test)
  add_executable(
    morpheus_gkstring_helper_context_test
    test/gkstring-helper-context.c
  )
  target_link_libraries(
    morpheus_gkstring_helper_context_test
    PRIVATE morpheus_morphlib morpheus_build_options
  )
  add_test(
    NAME gkstring_helper_context
    COMMAND morpheus_gkstring_helper_context_test
  )
  add_executable(morpheus_binlook_test test/binlook.c)
  target_link_libraries(
    morpheus_binlook_test
    PRIVATE morpheus_greeklib morpheus_build_options
  )
  add_test(NAME binlook COMMAND morpheus_binlook_test)
  set_tests_properties(
    runtime_context gkstring_helper_context binlook
    PROPERTIES ENVIRONMENT "MORPHLIB=${PROJECT_SOURCE_DIR}/stemlib"
  )
  if(MORPHEUS_ENABLE_SANITIZERS)
    set_property(
      TEST runtime_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST gkstring_helper_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST binlook APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
  endif()
endif()

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
target_compile_options(
  morpheus_gkends
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-int>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=incompatible-pointer-types>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=return-type>
)

if(BUILD_TESTING)
  add_executable(
    morpheus_contraction_context_test
    test/contraction-context.c
  )
  target_link_libraries(
    morpheus_contraction_context_test
    PRIVATE morpheus_gkends morpheus_morphlib morpheus_build_options
  )
  add_test(
    NAME contraction_context
    COMMAND morpheus_contraction_context_test
  )
  add_executable(
    morpheus_ending_cache_context_test
    test/ending-cache-context.c
  )
  target_link_libraries(
    morpheus_ending_cache_context_test
    PRIVATE morpheus_gkends morpheus_morphlib morpheus_build_options
  )
  add_test(
    NAME ending_cache_context
    COMMAND morpheus_ending_cache_context_test
  )
  set_tests_properties(
    contraction_context ending_cache_context
    PROPERTIES ENVIRONMENT "MORPHLIB=${PROJECT_SOURCE_DIR}/stemlib"
  )
  if(MORPHEUS_ENABLE_SANITIZERS)
    set_property(
      TEST contraction_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST ending_cache_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
  endif()
endif()

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
target_compile_options(
  morpheus_gkdict
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-int>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=incompatible-pointer-types>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=return-type>
)

if(BUILD_TESTING)
  add_executable(
    morpheus_dictionary_context_test
    test/dictionary-context.c
  )
  target_link_libraries(
    morpheus_dictionary_context_test
    PRIVATE morpheus_gkdict morpheus_morphlib morpheus_build_options
  )
  add_test(
    NAME dictionary_context
    COMMAND morpheus_dictionary_context_test
  )
  add_executable(
    morpheus_derivation_context_test
    test/derivation-context.c
  )
  target_link_libraries(
    morpheus_derivation_context_test
    PRIVATE morpheus_gkdict morpheus_morphlib morpheus_build_options
  )
  add_test(
    NAME derivation_context
    COMMAND morpheus_derivation_context_test
  )
  add_executable(
    morpheus_compound_head_context_test
    test/compound-head-context.c
  )
  target_link_libraries(
    morpheus_compound_head_context_test
    PRIVATE morpheus_gkdict morpheus_morphlib morpheus_build_options
  )
  add_test(
    NAME compound_head_context
    COMMAND morpheus_compound_head_context_test
  )
  add_executable(
    morpheus_hq_dictionary_context_test
    test/hq-dictionary-context.c
  )
  target_link_libraries(
    morpheus_hq_dictionary_context_test
    PRIVATE morpheus_gkdict morpheus_morphlib morpheus_build_options
  )
  add_test(
    NAME hq_dictionary_context
    COMMAND morpheus_hq_dictionary_context_test
  )
  set_tests_properties(
    dictionary_context derivation_context compound_head_context
    hq_dictionary_context
    PROPERTIES ENVIRONMENT "MORPHLIB=${PROJECT_SOURCE_DIR}/stemlib"
  )
  if(MORPHEUS_ENABLE_SANITIZERS)
    set_property(
      TEST dictionary_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST derivation_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST compound_head_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST hq_dictionary_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
  endif()
endif()

add_library(morpheus_gener STATIC src/gener/genwd.c)
target_link_libraries(
  morpheus_gener
  PUBLIC morpheus_build_options
  PRIVATE morpheus_gkends morpheus_morphlib morpheus_greeklib
)
target_compile_options(
  morpheus_gener
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-int>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=incompatible-pointer-types>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=return-type>
)

if(BUILD_TESTING)
  add_executable(
    morpheus_dictionary_entry_workspace_test
    test/dictionary-entry-workspace.c
  )
  target_link_libraries(
    morpheus_dictionary_entry_workspace_test
    PRIVATE
      morpheus_gener
      morpheus_gkends
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME dictionary_entry_workspace
    COMMAND morpheus_dictionary_entry_workspace_test
  )
  set_tests_properties(
    dictionary_entry_workspace
    PROPERTIES ENVIRONMENT "MORPHLIB=${PROJECT_SOURCE_DIR}/stemlib"
  )
  if(MORPHEUS_ENABLE_SANITIZERS)
    set_property(
      TEST dictionary_entry_workspace APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
  endif()
endif()

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
target_compile_options(
  morpheus_anal
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=implicit-int>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=incompatible-pointer-types>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror=return-type>
)

if(BUILD_TESTING)
  add_executable(
    morpheus_analysis_output_context_test
    test/analysis-output-context.c
  )
  target_link_libraries(
    morpheus_analysis_output_context_test
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME analysis_output_context
    COMMAND morpheus_analysis_output_context_test
  )
  add_executable(
    morpheus_stem_buffer_context_test
    test/stem-buffer-context.c
  )
  target_link_libraries(
    morpheus_stem_buffer_context_test
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME stem_buffer_context
    COMMAND morpheus_stem_buffer_context_test
  )
  add_executable(
    morpheus_irregular_buffer_context_test
    test/irregular-buffer-context.c
  )
  target_link_libraries(
    morpheus_irregular_buffer_context_test
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME irregular_buffer_context
    COMMAND morpheus_irregular_buffer_context_test
  )
  add_executable(
    morpheus_crasis_option_context_test
    test/crasis-option-context.c
  )
  target_link_libraries(
    morpheus_crasis_option_context_test
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME crasis_option_context
    COMMAND morpheus_crasis_option_context_test
  )
  add_executable(
    morpheus_quick_option_context_test
    test/quick-option-context.c
  )
  target_link_libraries(
    morpheus_quick_option_context_test
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME quick_option_context
    COMMAND morpheus_quick_option_context_test
  )
  add_executable(
    morpheus_ending_match_context_test
    test/ending-match-context.c
  )
  target_link_libraries(
    morpheus_ending_match_context_test
    PRIVATE
      morpheus_gkends
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME ending_match_context
    COMMAND morpheus_ending_match_context_test
  )
  add_executable(
    morpheus_suffix_stream_context_test
    test/suffix-stream-context.c
  )
  target_link_libraries(
    morpheus_suffix_stream_context_test
    PRIVATE
      morpheus_gkends
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME suffix_stream_context
    COMMAND morpheus_suffix_stream_context_test
  )
  add_executable(
    morpheus_ending_store_context_test
    test/ending-store-context.c
  )
  target_link_libraries(
    morpheus_ending_store_context_test
    PRIVATE
      morpheus_gkends
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME ending_store_context
    COMMAND morpheus_ending_store_context_test
  )
  add_executable(
    morpheus_end_index_context_test
    test/end-index-context.c
  )
  target_link_libraries(
    morpheus_end_index_context_test
    PRIVATE
      morpheus_gkends
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME end_index_context
    COMMAND morpheus_end_index_context_test
  )
  add_executable(
    morpheus_analysis_stats_context_test
    test/analysis-stats-context.c
  )
  target_link_libraries(
    morpheus_analysis_stats_context_test
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME analysis_stats_context
    COMMAND morpheus_analysis_stats_context_test
  )
  add_executable(
    morpheus_ending_workspace_context_test
    test/ending-workspace-context.c
  )
  target_link_libraries(
    morpheus_ending_workspace_context_test
    PRIVATE
      morpheus_gkends
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME ending_workspace_context
    COMMAND morpheus_ending_workspace_context_test
  )
  add_executable(
    morpheus_dialect_option_context_test
    test/dialect-option-context.c
  )
  target_link_libraries(
    morpheus_dialect_option_context_test
    PRIVATE
      morpheus_anal
      morpheus_gener
      morpheus_gkends
      morpheus_gkdict
      morpheus_morphlib
      morpheus_greeklib
      morpheus_build_options
  )
  add_test(
    NAME dialect_option_context
    COMMAND morpheus_dialect_option_context_test
  )
  set_tests_properties(
    analysis_output_context stem_buffer_context irregular_buffer_context
    crasis_option_context quick_option_context ending_match_context suffix_stream_context
    ending_store_context end_index_context analysis_stats_context
    ending_workspace_context dialect_option_context
    PROPERTIES ENVIRONMENT "MORPHLIB=${PROJECT_SOURCE_DIR}/stemlib"
  )
  if(MORPHEUS_ENABLE_SANITIZERS)
    set_property(
      TEST analysis_output_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST stem_buffer_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST irregular_buffer_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST crasis_option_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST quick_option_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST ending_match_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST suffix_stream_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST ending_store_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST end_index_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST analysis_stats_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST ending_workspace_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
    set_property(
      TEST dialect_option_context APPEND PROPERTY ENVIRONMENT
        "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
    )
  endif()
endif()

add_library(morpheus SHARED src/api/context.c src/api/result.c src/api/analyze.c src/api/compat.c)
add_library(Morpheus::morpheus ALIAS morpheus)
target_include_directories(
  morpheus
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE "${PROJECT_SOURCE_DIR}/src/includes"
)
target_link_libraries(
  morpheus
  PRIVATE
    morpheus_anal
    morpheus_gener
    morpheus_gkends
    morpheus_gkdict
    morpheus_morphlib
    morpheus_greeklib
    morpheus_build_options
)
set_target_properties(
  morpheus
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME morpheus
)
install(
  TARGETS morpheus
  EXPORT MorpheusTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
  FILES include/morpheus/morpheus.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/morpheus
)
configure_package_config_file(
  cmake/MorpheusConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/MorpheusConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Morpheus"
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/MorpheusConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(
  EXPORT MorpheusTargets
  NAMESPACE Morpheus::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Morpheus"
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MorpheusConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MorpheusConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Morpheus"
)
configure_file(
  cmake/libmorpheus.pc.in
  "${CMAKE_CURRENT_BINARY_DIR}/libmorpheus.pc"
  @ONLY
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/libmorpheus.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)
if(BUILD_TESTING)
  add_executable(morpheus_public_context_test test/public-context.c)
  target_link_libraries(
    morpheus_public_context_test PRIVATE morpheus morpheus_build_options
  )
  add_test(NAME public_context COMMAND morpheus_public_context_test)
  add_executable(morpheus_public_result_test test/public-result.c)
  target_link_libraries(
    morpheus_public_result_test PRIVATE morpheus morpheus_build_options
  )
  add_test(NAME public_result COMMAND morpheus_public_result_test)
  add_executable(morpheus_public_compat_test test/public-compat.c)
  target_compile_definitions(
    morpheus_public_compat_test
    PRIVATE MORPHEUS_TEST_STEMLIB="${PROJECT_SOURCE_DIR}/stemlib"
  )
  target_link_libraries(
    morpheus_public_compat_test PRIVATE morpheus morpheus_build_options
  )
  add_test(NAME public_compat COMMAND morpheus_public_compat_test)
  add_executable(morpheus_public_analysis_test test/public-analysis.c)
  target_compile_definitions(
    morpheus_public_analysis_test
    PRIVATE MORPHEUS_TEST_STEMLIB="${PROJECT_SOURCE_DIR}/stemlib"
  )
  target_link_libraries(
    morpheus_public_analysis_test PRIVATE morpheus morpheus_build_options
  )
  add_test(NAME public_analysis COMMAND morpheus_public_analysis_test)
  add_executable(
    morpheus_public_context_isolation_test
    test/public-context-isolation.c
  )
  target_compile_definitions(
    morpheus_public_context_isolation_test
    PRIVATE MORPHEUS_TEST_STEMLIB="${PROJECT_SOURCE_DIR}/stemlib"
  )
  target_link_libraries(
    morpheus_public_context_isolation_test
    PRIVATE morpheus morpheus_build_options
  )
  add_test(
    NAME public_context_isolation
    COMMAND morpheus_public_context_isolation_test
  )
  add_executable(morpheus_public_fixture_test test/public-fixtures.c)
  target_compile_definitions(
    morpheus_public_fixture_test
    PRIVATE MORPHEUS_TEST_STEMLIB="${PROJECT_SOURCE_DIR}/stemlib"
  )
  target_link_libraries(
    morpheus_public_fixture_test PRIVATE morpheus morpheus_build_options
  )
  add_test(NAME public_fixtures COMMAND morpheus_public_fixture_test)
  add_executable(
    morpheus_public_parallel_context_test
    test/public-parallel-contexts.c
  )
  target_compile_definitions(
    morpheus_public_parallel_context_test
    PRIVATE MORPHEUS_TEST_STEMLIB="${PROJECT_SOURCE_DIR}/stemlib"
  )
  target_link_libraries(
    morpheus_public_parallel_context_test
    PRIVATE morpheus morpheus_build_options Threads::Threads
  )
  add_test(
    NAME public_parallel_contexts
    COMMAND morpheus_public_parallel_context_test
  )
  if(NOT MORPHEUS_ENABLE_SANITIZERS AND
     NOT MORPHEUS_ENABLE_THREAD_SANITIZER)
    add_test(
      NAME installed_package_consumer
      COMMAND
        "${CMAKE_COMMAND}"
        "-DMORPHEUS_BUILD_DIR=${PROJECT_BINARY_DIR}"
        "-DMORPHEUS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
        "-DMORPHEUS_GENERATOR=${CMAKE_GENERATOR}"
        -P "${PROJECT_SOURCE_DIR}/test/test-installed-package.cmake"
    )
    find_program(MORPHEUS_PKG_CONFIG_EXECUTABLE pkg-config)
    if(MORPHEUS_PKG_CONFIG_EXECUTABLE)
      add_test(
        NAME installed_pkg_config
        COMMAND
          "${CMAKE_COMMAND}"
          "-DMORPHEUS_BUILD_DIR=${PROJECT_BINARY_DIR}"
          "-DMORPHEUS_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}"
          "-DMORPHEUS_PKG_CONFIG=${MORPHEUS_PKG_CONFIG_EXECUTABLE}"
          "-DMORPHEUS_VERSION=${PROJECT_VERSION}"
          -P "${PROJECT_SOURCE_DIR}/test/test-installed-pkgconfig.cmake"
      )
    endif()
  endif()
endif()

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
      morpheus
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
