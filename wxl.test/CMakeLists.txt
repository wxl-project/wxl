# ---- CMakeList.txt : wxl.test ----
#
# What the tests of wxl are written with, and those of a project that adds wxl:
#
#   wxl::test        headers alone, linked by whoever needs them: sta_box.h, the stress-test harness
#                    (stress_test.h: STRESS_TEST_CASE, become_true, the timeouts the timing-sensitive
#                    tests agree on) and the running test's own directory (test_directory.h). The
#                    target brings no GoogleTest, and sta_box.h includes none, so a program without
#                    it -- a benchmark -- may use that header; the other two include gtest and are
#                    for a test, which links wxl::test.gtest.
#   wxl::test.gtest  the global fixture of a GoogleTest program (test_initializer.cpp): the C
#                    library's locale set to the user's before the tests (setlocale(LC_ALL, "")),
#                    and, on a failed CRT debug assertion (_ASSERTE), the call stack printed with
#                    symbols and the process ended, as assert() of the UCRT ends it. Brings
#                    wxl::test and GTest::gtest; the program takes its main() from GTest::gtest_main,
#                    as wxl_add_gtest links it, or defines its own.
#
# The fixture is an object library rather than a static one: a gtest environment registers itself
# from the initializer of a global, which nothing in the program refers to, and the linker takes from
# a static library only the objects that something refers to. The objects of an object library go
# into each target that links it directly -- through another library's usage requirements they do
# not -- so a test program names wxl::test.gtest itself.
#
# Both travel with an installed wxl (cmake/wxl_install.cmake): wxl::test exported with its headers,
# and the source of the fixture, which the project compiles over its own GoogleTest.
add_library(wxl.test INTERFACE)
add_library(wxl::test ALIAS wxl.test)

target_sources(wxl.test
    INTERFACE
        FILE_SET headers TYPE HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES sta_box.h stress_test.h test_directory.h
)

target_link_libraries(wxl.test INTERFACE wxl::core)

set_target_properties(wxl.test PROPERTIES
    EXPORT_NAME test
    WXL_TEST_GTEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/test_initializer.cpp"
)

# Where there is GoogleTest: the tree's own when its tests are built (cmake/wxl_gtest.cmake, WXL_TESTS
# at the root), or one a project that adds wxl made available before it (cmake/wxl_add_gtest.cmake).
_wxl_add_test_gtest()
