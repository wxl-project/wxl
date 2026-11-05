# ---- GoogleTest, built inside the tree ----
#
# Included by the root of the tree under WXL_TESTS, or by a project that adds
# wxl and wants its GoogleTest, before the add_subdirectory of wxl. The guard
# makes a second include a no-op, so the root reuses what such a project made
# available.
#
# GoogleTest is built from its own sources, at a pinned tag, with the language
# standard and runtime of the configure that includes this file, not taken
# prebuilt: gtest declares its char8_t printers (PrintU8StringTo) only when it
# is compiled as C++20 or later, so a library built as C++17 leaves every test
# that prints a u8string_view with an unresolved symbol. Every machine and CI
# get the same tag, and there is no package to install for it.
#
# OVERRIDE_FIND_PACKAGE: always the fetched source, never an installed package
# that happens to be found first. FetchContent then answers
# find_package(googletest) from CMAKE_FIND_PACKAGE_REDIRECTS_DIR, the directory
# find_package searches before anything else, and the gtest-config files
# written below forward find_package(GTest) -- the name every CMakeLists.txt
# here asks for -- to it. GTest::gtest and GTest::gtest_main are googletest's
# own aliases.
#
# gtest_force_shared_crt follows the CRT of the configure,
# CMAKE_MSVC_RUNTIME_LIBRARY: ON for the /MD family -- what the x64-dynamic
# preset sets, and CMake's default when the variable is not set or empty -- and
# OFF for the /MT family, which the x64-static preset sets. gtest picks the
# static CRT itself only when it is the top-level project and the variable is
# not defined (googletest/cmake/internal_utils.cmake, read at v1.18.0);
# otherwise its targets take the variable like any other, and the option states
# the same CRT instead of contradicting it. No gmock and no install rules:
# nothing here mocks, and nothing installs gtest.

include_guard(GLOBAL)
include(FetchContent)

if("${CMAKE_MSVC_RUNTIME_LIBRARY}" STREQUAL "" OR CMAKE_MSVC_RUNTIME_LIBRARY MATCHES "DLL")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
else()
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
endif()
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
    GIT_SHALLOW TRUE
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(googletest)

# gtest imports nothing, so it is neither scanned for imports nor given a std
# module of its own, which a project-wide CMAKE_CXX_SCAN_FOR_MODULES and
# CMAKE_CXX_MODULE_STD would otherwise give its targets.
set_target_properties(gtest gtest_main PROPERTIES
    CXX_SCAN_FOR_MODULES OFF
    CXX_MODULE_STD OFF
)

set(_redirects "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}")
if(NOT EXISTS "${_redirects}/gtest-config.cmake" AND
   NOT EXISTS "${_redirects}/GTestConfig.cmake")
    file(WRITE "${_redirects}/gtest-config.cmake" [=[
include(CMakeFindDependencyMacro)
find_dependency(googletest)
]=])
endif()
if(NOT EXISTS "${_redirects}/gtest-config-version.cmake" AND
   NOT EXISTS "${_redirects}/GTestConfigVersion.cmake")
    file(WRITE "${_redirects}/gtest-config-version.cmake" [=[
include("${CMAKE_CURRENT_LIST_DIR}/googletest-config-version.cmake")
]=])
endif()
unset(_redirects)
