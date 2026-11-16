// The test program of install-core: declared with wxl_add_gtest of an installed
// wxl, over the GoogleTest of its wxl_gtest.cmake, with the fixture
// wxl::test.gtest compiled from the installed source and the headers of
// wxl::test.

#include <gtest/gtest.h>
#include <locale.h>

#include "test_directory.h"

import std;
import wxl.core;

// The fixture's environment set the C library's locale to the user's before
// the tests; without it the locale is the "C" every program starts with.
TEST(InstallConsumer, FixtureSetsTheLocale) {
    EXPECT_STRNE(setlocale(LC_ALL, nullptr), "C");
}

TEST(InstallConsumer, TestDirectory) {
    const wxl::core::path directory = wxl::core::test_directory();
    EXPECT_NE(directory.native().find(L"wxl.InstallConsumer.TestDirectory-"), std::wstring_view::npos);
}
