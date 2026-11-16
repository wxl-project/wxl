# An application outside the wxl tree that takes wxl from the NuGet package
# WinXL, as an application on wxl does: its preset includes the provider
# cmake/winxl-provider.cmake of wxl, which restores the package for
# find_package(wxl) below, with nuget.config beside this file and the lock file
# packages.lock.json it writes here.
#
# The job consumer of .github/workflows/package.yml copies this directory out
# of the checkout, puts the provider into its cmake/, publishes the fresh
# package into its feed/, the local source of nuget.config, with
# tools/package/publish.cmake, and configures and builds it with the presets:
# md, the CRT in a DLL in Debug and Release, and mixed, the CRT in a DLL in
# Debug and the static one in Release. The same by hand, from a developer
# prompt in such a copy:
#
#     cmake -DWXL_PUBLISH_PACKAGE=<WinXL.<version>.nupkg> -DWXL_PUBLISH_FEED=feed
#           -P <wxl>/tools/package/publish.cmake
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
#              and wxl::ui.common of the package is linked. The profile extends
#              wxl's extended, which is not beside it: the generator and the
#              configure find it, and base under it, among the profiles of the
#              package. extended selects the components bb, rsdn and code, and
#              the window shows a block of each, so the modules of wxl.bb,
#              wxl.rsdn and wxl.highlight are built here from the interfaces of
#              the package and their archives are linked, with the CRT of each
#              configuration. Its own lists are those of base.
cmake_minimum_required(VERSION 4.5)

project(wxl_package_consumer LANGUAGES CXX)

# The language of wxl, for the whole directory and before the package: the
# module interfaces of wxl are built with the options of the application.
add_compile_options(/permissive- /utf-8 /Zc:throwingNew)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
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
