# An application outside the wxl tree that takes wxl from the NuGet package
# WinXL, as an application on wxl does: its preset includes the provider
# cmake/winxl-provider.cmake of wxl, which restores the package for
# find_package(wxl) below, with nuget.config beside this file and the lock file
# packages.lock.json it writes here.
#
# The job consumer of .github/workflows/package.yml copies this directory out
# of the checkout, puts the provider into its cmake/ and the fresh package into
# its packages/, the local source of nuget.config, and configures and builds it
# with the presets: md, the CRT in a DLL in Debug and Release, and mixed, the
# CRT in a DLL in Debug and the static one in Release. The same by hand, from a
# developer prompt in such a copy:
#
#     cmake --preset md
#     cmake --build --preset md-debug
#
# What it builds:
#
#   core-only  a console program on wxl.core alone, its module and that of fmt
#              built here from the interfaces of the package; it prints the
#              version of the package.
#   demo       a window on the library of the profile demo, profile/
#              demo.winxl-profile: the generator of the package writes the
#              wrappers, the components of the package are compiled with them,
#              and wxl::ui.common of the package is linked. The profile is a copy
#              of wxl's base and extends nothing, so that it needs nothing
#              outside the package.
cmake_minimum_required(VERSION 4.3)

# The `import std;` gate opens before project(); its UUID is that of the CMake
# release that configures (Help/dev/experimental.rst of CMake).
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.4 AND CMAKE_VERSION VERSION_LESS 4.5)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.3 AND CMAKE_VERSION VERSION_LESS 4.4)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
else()
    message(FATAL_ERROR "No CMAKE_EXPERIMENTAL_CXX_IMPORT_STD UUID here for CMake ${CMAKE_VERSION}.")
endif()

project(wxl_package_consumer LANGUAGES CXX)

# The language of wxl, for the whole directory and before the package: the
# module interfaces of wxl are built with the options of the application.
add_compile_options(/permissive- /utf-8 /Zc:throwingNew)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# The least version of WinXL the application accepts, and the one the provider
# restores.
find_package(wxl 0.1 CONFIG REQUIRED)

add_executable(core-only core-only.cpp)
target_link_libraries(core-only PRIVATE wxl::core)

wxl_add_ui_library(demo PROFILE "${CMAKE_CURRENT_SOURCE_DIR}/profile/demo.winxl-profile")

wxl_add_executable(demo
    UI wxl::ui.demo
    demo.cpp)
