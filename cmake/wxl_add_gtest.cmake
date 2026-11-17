# /cmake/wxl_add_gtest.cmake

include_guard(GLOBAL)

# gtest_discover_tests, for a project that adds wxl and declares its tests with
# wxl_add_gtest without having included the module itself.
include(GoogleTest)

macro(wxl_add_gtest target)
    cmake_parse_arguments(ARG "" "" "SRC;DEPS" ${ARGN})

    add_executable(${target} ${ARG_SRC})

    target_link_libraries(${target}
        PRIVATE
            GTest::gtest_main
            ${ARG_DEPS}
    )

    # A test target links wxl.core, and consuming a module built with
    # `import std;` means being scanned, which gives it the std module too.
    set_target_properties(${target} PROPERTIES
        CXX_SCAN_FOR_MODULES ON
    )

    wxl_set_target_folders(${target} PREFIX "tests")

    # Listed when ctest starts, from the binary of the configuration it tests. A
    # program that cannot list itself stops ctest with an error, as it would
    # stop the build. The listing may take up to a minute: a CI runner can take
    # longer than the default five seconds to start a large program, and ctest
    # goes on as soon as the program has answered.
    gtest_discover_tests(${target} DISCOVERY_MODE PRE_TEST DISCOVERY_TIMEOUT 60)
endmacro()

# wxl::test.gtest, the global fixture of a GoogleTest program
# (wxl.test/CMakeLists.txt), made once both wxl::test and GTest::gtest are
# there, by whichever of the two comes second: wxl.test after a project's
# GoogleTest in the tree, and for an installed wxl its configuration, or
# wxl_gtest.cmake included after find_package(wxl). The source is the one the
# property WXL_TEST_GTEST_SOURCE of wxl::test names, the tree's or the
# installed copy.
#
# An object library, out of ALL: a gtest environment registers itself from
# the initializer of a global that nothing refers to, so the objects go into
# each program that links the fixture itself, and a project that has
# GoogleTest without linking the fixture does not compile it.
function(_wxl_add_test_gtest)
    if(TARGET wxl.test.gtest OR NOT TARGET wxl::test OR NOT TARGET GTest::gtest)
        return()
    endif()
    get_target_property(source wxl::test WXL_TEST_GTEST_SOURCE)
    add_library(wxl.test.gtest OBJECT "${source}")
    add_library(wxl::test.gtest ALIAS wxl.test.gtest)
    # wxl::core besides wxl::test, which brings it already: for an installed
    # wxl, CMake builds the BMI of an imported module for the targets that link
    # it themselves, and not for those that reach it through an imported
    # library of headers.
    target_link_libraries(wxl.test.gtest PUBLIC GTest::gtest wxl::test wxl::core)

    # The fixture imports std and wxl.core: CMAKE_CXX_SCAN_FOR_MODULES gives it
    # the scan and with it the std module, set by the root at the top level and
    # checked in a project that adds or installs wxl.
    set_target_properties(wxl.test.gtest PROPERTIES EXCLUDE_FROM_ALL ON)
endfunction()
