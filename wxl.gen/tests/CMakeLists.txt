wxl_add_gtest(wxl.gen.tests
    SRC
        doc_language_test.cpp
        documentation_ids_test.cpp
        dsl_test.cpp
        emit_test.cpp
        generate_test.cpp
        group_namespaces_test.cpp
        map_type_test.cpp
        parts_test.cpp
    DEPS
        wxl.gen
        # test_directory() for the tests that write the generator's output.
        wxl::test.gtest
)

# The type mapping is tested on the generator's real profiles and the metadata
# they name, which is where the generator's output comes from: the packages and
# the Windows SDK of this build, as its configure lists them.
wxl_restored_packages_file(_wxl_gen_restored_packages)
target_compile_definitions(wxl.gen.tests
    PRIVATE
        WXL_GEN_PROFILES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../profiles"
        WXL_UI_COMPONENTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../wxl.ui/components"
        WXL_RESTORED_PACKAGES="${_wxl_gen_restored_packages}"
)

# The runs below read the tree's base profile, which names components: they
# are given the component directory, the chain's components and the common
# directory the way the build gives them (cmake/wxl_ui_profile.cmake).
include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/wxl_ui_profile.cmake")
set(_wxl_gen_components_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../wxl.ui/components")
wxl_read_profile_components(_wxl_gen_components
    "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/base.winxl-profile" "${_wxl_gen_components_dir}")
set(_wxl_gen_component_args --component-dir "${_wxl_gen_components_dir}")
foreach(_wxl_gen_component IN LISTS _wxl_gen_components_CHAIN)
    list(APPEND _wxl_gen_component_args --component "${_wxl_gen_component}")
endforeach()
list(APPEND _wxl_gen_component_args --common-dir "${CMAKE_CURRENT_SOURCE_DIR}/../../wxl.ui/src")

# The arguments are parsed in winui-srcgen itself, not in the library, so the
# refusal of a language the reference has no folder for is tested on the
# executable: one test for the message, one for the failing exit code --
# PASS_REGULAR_EXPRESSION alone ignores the code, and WILL_FAIL would invert
# the match. The refusal comes before the profile is read.
set(_wxl_gen_doc_language
    winui-srcgen --profile "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/base.winxl-profile"
    --packages "${_wxl_gen_restored_packages}"
    ${_wxl_gen_component_args}
    --docs --doc-language)
add_test(NAME winui-srcgen.unknown-doc-language.message COMMAND ${_wxl_gen_doc_language} pt)
set_tests_properties(winui-srcgen.unknown-doc-language.message PROPERTIES
    PASS_REGULAR_EXPRESSION "--doc-language pt is not a language of the reference")
add_test(NAME winui-srcgen.unknown-doc-language.fails COMMAND ${_wxl_gen_doc_language} pt)
set_tests_properties(winui-srcgen.unknown-doc-language.fails PROPERTIES WILL_FAIL TRUE)

# An empty code is refused by the same message, which leaves two spaces where
# the code stands. It is a quoted argument after the list: an empty element
# inside a list is dropped when the list is expanded.
add_test(NAME winui-srcgen.empty-doc-language.message COMMAND ${_wxl_gen_doc_language} "")
set_tests_properties(winui-srcgen.empty-doc-language.message PROPERTIES
    PASS_REGULAR_EXPRESSION "--doc-language  is not a language of the reference")
add_test(NAME winui-srcgen.empty-doc-language.fails COMMAND ${_wxl_gen_doc_language} "")
set_tests_properties(winui-srcgen.empty-doc-language.fails PROPERTIES WILL_FAIL TRUE)

# A run without the list of the restored packages stops before it reads a
# profile, and says where the list comes from.
set(_wxl_gen_no_packages
    winui-srcgen --profile "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/base.winxl-profile"
    ${_wxl_gen_component_args})
add_test(NAME winui-srcgen.no-packages.message COMMAND ${_wxl_gen_no_packages})
set_tests_properties(winui-srcgen.no-packages.message PROPERTIES
    PASS_REGULAR_EXPRESSION "--packages <build>/wxl.packages/restored-packages.json")
add_test(NAME winui-srcgen.no-packages.fails COMMAND ${_wxl_gen_no_packages})
set_tests_properties(winui-srcgen.no-packages.fails PROPERTIES WILL_FAIL TRUE)

# A list of the build's components with no directory to find them in is refused
# before the profile is read, not dropped.
set(_wxl_gen_no_component_dir
    winui-srcgen --profile "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/base.winxl-profile"
    --packages "${_wxl_gen_restored_packages}" --out "${CMAKE_CURRENT_BINARY_DIR}/components-refused"
    --component window --component dsl)
add_test(NAME winui-srcgen.component-without-dir.message COMMAND ${_wxl_gen_no_component_dir})
set_tests_properties(winui-srcgen.component-without-dir.message PROPERTIES
    PASS_REGULAR_EXPRESSION
    "the build names components \\(--component: dsl, window\\) and no directory to find them in \\(--component-dir\\)")
add_test(NAME winui-srcgen.component-without-dir.fails COMMAND ${_wxl_gen_no_component_dir})
set_tests_properties(winui-srcgen.component-without-dir.fails PROPERTIES WILL_FAIL TRUE)

# A number of parts the names cannot carry -- they have two digits -- or no number
# at all is refused before the profile is read, as the language is.
set(_wxl_gen_parts
    winui-srcgen --profile "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/base.winxl-profile"
    --packages "${_wxl_gen_restored_packages}" --out "${CMAKE_CURRENT_BINARY_DIR}/parts-refused"
    ${_wxl_gen_component_args}
    --parts)
foreach(_wxl_gen_count IN ITEMS 0 101 eight)
    add_test(NAME winui-srcgen.parts-${_wxl_gen_count}.message
        COMMAND ${_wxl_gen_parts} ${_wxl_gen_count})
    set_tests_properties(winui-srcgen.parts-${_wxl_gen_count}.message PROPERTIES
        PASS_REGULAR_EXPRESSION "--parts ${_wxl_gen_count}: the number of parts is from 1 to 100")
    add_test(NAME winui-srcgen.parts-${_wxl_gen_count}.fails
        COMMAND ${_wxl_gen_parts} ${_wxl_gen_count})
    set_tests_properties(winui-srcgen.parts-${_wxl_gen_count}.fails PROPERTIES WILL_FAIL TRUE)
endforeach()

# A binding pair of a type map that is not what the key says stops the run with
# the file and the pair named: a key the entry does not have as the map is read,
# an event its class does not declare once the metadata is open. Both stop before
# anything is written.
set(_wxl_gen_type_map
    winui-srcgen --profile "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/base.winxl-profile"
    --packages "${_wxl_gen_restored_packages}" --out "${CMAKE_CURRENT_BINARY_DIR}/pair-refused"
    ${_wxl_gen_component_args}
    --types)
set(_wxl_gen_pair_with_an_unknown_key
    "binding pair Microsoft.UI.Xaml.Controls.ToggleSwitch.IsOn: 'event' is not a key of this entry")
set(_wxl_gen_pair_under_an_event_of_another_class
    "binding pair Microsoft.UI.Xaml.Controls.ToggleSwitch.IsOn: Microsoft.UI.Xaml.Controls.ToggleSwitch declares no event Checked")
foreach(_wxl_gen_case IN ITEMS pair-with-an-unknown-key pair-under-an-event-of-another-class)
    string(REPLACE "-" "_" _wxl_gen_message "_wxl_gen_${_wxl_gen_case}")
    set(_wxl_gen_command
        ${_wxl_gen_type_map} "${CMAKE_CURRENT_SOURCE_DIR}/type-maps/${_wxl_gen_case}.json")
    add_test(NAME winui-srcgen.${_wxl_gen_case}.message COMMAND ${_wxl_gen_command})
    set_tests_properties(winui-srcgen.${_wxl_gen_case}.message PROPERTIES
        PASS_REGULAR_EXPRESSION "${${_wxl_gen_message}}")
    add_test(NAME winui-srcgen.${_wxl_gen_case}.fails COMMAND ${_wxl_gen_command})
    set_tests_properties(winui-srcgen.${_wxl_gen_case}.fails PROPERTIES WILL_FAIL TRUE)
endforeach()

# A command line the run cannot carry out stops it with a message that says what is
# wrong, and the run fails: an option it does not know, an option without its value,
# a language of the documentation without the documentation, no output directory, a
# metadata file or a table of symbols named and not there, and profiles that leave
# nothing to read or nothing to walk.
function(_wxl_gen_refusal name message)
    add_test(NAME winui-srcgen.${name}.message COMMAND winui-srcgen ${ARGN})
    set_tests_properties(winui-srcgen.${name}.message PROPERTIES
        PASS_REGULAR_EXPRESSION "${message}")
    add_test(NAME winui-srcgen.${name}.fails COMMAND winui-srcgen ${ARGN})
    set_tests_properties(winui-srcgen.${name}.fails PROPERTIES WILL_FAIL TRUE)
endfunction()
set(_wxl_gen_base --profile "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/base.winxl-profile")
set(_wxl_gen_run
    --packages "${_wxl_gen_restored_packages}" --out "${CMAKE_CURRENT_BINARY_DIR}/run-refused"
    --types "${CMAKE_CURRENT_SOURCE_DIR}/../profiles/types.json")
_wxl_gen_refusal(unknown-argument "unknown argument: --sauce" ${_wxl_gen_base} --sauce)
_wxl_gen_refusal(option-without-value "--profile needs a value" --profile)
_wxl_gen_refusal(doc-language-without-docs "--doc-language needs --docs"
    ${_wxl_gen_base} --doc-language en)
_wxl_gen_refusal(no-output-directory "no output directory: pass --out <dir>"
    ${_wxl_gen_base} --packages "${_wxl_gen_restored_packages}")
_wxl_gen_refusal(missing-winmd "no such metadata file: .*no-such\\.winmd"
    ${_wxl_gen_base} ${_wxl_gen_run} ${_wxl_gen_component_args}
    --winmd "${CMAKE_CURRENT_BINARY_DIR}/no-such.winmd")
_wxl_gen_refusal(missing-symbols "cannot open: .*no-such-symbols\\.json"
    ${_wxl_gen_base} ${_wxl_gen_run}
    --symbols "${CMAKE_CURRENT_BINARY_DIR}/no-such-symbols.json")
_wxl_gen_refusal(nothing-to-read "no metadata to read: the profiles name no packages"
    --profile "${CMAKE_CURRENT_SOURCE_DIR}/profiles/nothing-to-read.winxl-profile" ${_wxl_gen_run})
_wxl_gen_refusal(nothing-to-walk "no roots to walk: the profiles list no types"
    --profile "${CMAKE_CURRENT_SOURCE_DIR}/profiles/nothing-to-walk.winxl-profile" ${_wxl_gen_run})

# Every test above runs the winui-srcgen executable, and the setup of the
# documentation site's test (docs.language, docs/CMakeLists.txt) builds the site's
# generation, which may link that executable anew. Windows does not let a running
# executable be replaced, so under ctest -j the link would fail while one of these
# runs; the shared lock keeps them apart.
get_property(_wxl_gen_tests DIRECTORY PROPERTY TESTS)
list(FILTER _wxl_gen_tests INCLUDE REGEX "^winui-srcgen\\.")
set_tests_properties(${_wxl_gen_tests} PROPERTIES RESOURCE_LOCK winui-srcgen)
