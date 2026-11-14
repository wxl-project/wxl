# /tools/package/pack.cmake
#
# Packs an installed wxl into the NuGet package WinXL:
#
#     cmake -DWXL_PACK_STAGE=<prefix> -DWXL_PACK_OUTPUT=<directory>
#           [-DWXL_PACK_COMMIT=<hash>] -P tools/package/pack.cmake
#
# <prefix> is what cmake --install of the tree wrote (cmake/wxl_install.cmake),
# and it becomes build/native of the package: the configuration is then at
# build/native/lib/cmake/wxl, and an application finds it with
#
#     find_package(wxl CONFIG REQUIRED PATHS <package>/build/native NO_DEFAULT_PATH)
#
# or with <package>/build/native in CMAKE_PREFIX_PATH. The install writes the
# MSBuild files of the package at the root of the prefix, WinXL.props and
# WinXL.targets (cmake/wxl_msbuild.cmake), so they land in build/native, where
# NuGet imports them into a .vcxproj that references the package. The package
# of a release is packed from a prefix both builds of the tree were installed
# into, the CRT in a DLL and the static one, with the tool set of the static
# build in bin; what the prefix holds is packed as it is, and the workflow that
# makes it
# (.github/workflows/package.yml) checks that.
#
# Read from the prefix, so the package says what the install says:
#
#   the version    PACKAGE_VERSION of lib/cmake/wxl/wxl-config-version.cmake,
#                  the version of the commit (cmake/wxl_package_version.cmake);
#   the dependencies  the NuGet packages of share/wxl/packages.json, the pins
#                  of the tree at the versions its restore resolved, each the
#                  least version the package asks for.
#
# The commit is WXL_PACK_COMMIT, or HEAD of the tree this script is in. The
# manifest is written from WinXL.nuspec.in into <directory>, and the package
# WinXL.<version>.nupkg beside it.
#
# The package is made by `dotnet pack` of the manifest itself, which the .NET
# SDK packs without a project since 10.0 (an SDK before it takes the file for
# a project and stops on its root element). global.json beside this script
# holds the SDK to a released 10.0 of the feature band 10.0.200 or a later
# one, which publish.cmake beside it needs as well; dotnet reads it from the
# working directory, which is this one.
#
# The package is published into a local feed by publish.cmake.

cmake_minimum_required(VERSION 3.28)

foreach(_wxl_arg WXL_PACK_STAGE WXL_PACK_OUTPUT)
    if(NOT ${_wxl_arg})
        message(FATAL_ERROR "pack.cmake: ${_wxl_arg} is not given.")
    endif()
endforeach()
cmake_path(ABSOLUTE_PATH WXL_PACK_STAGE NORMALIZE)
cmake_path(ABSOLUTE_PATH WXL_PACK_OUTPUT NORMALIZE)
string(REGEX REPLACE "/$" "" WXL_PACK_STAGE "${WXL_PACK_STAGE}")
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH _wxl_tools)
cmake_path(GET _wxl_tools PARENT_PATH _wxl_tree)

set(_wxl_version_file "${WXL_PACK_STAGE}/lib/cmake/wxl/wxl-config-version.cmake")
set(_wxl_pins_file "${WXL_PACK_STAGE}/share/wxl/packages.json")
foreach(_wxl_file IN ITEMS "${_wxl_version_file}" "${_wxl_pins_file}")
    if(NOT EXISTS "${_wxl_file}")
        message(FATAL_ERROR
            "pack.cmake: no ${_wxl_file}. ${WXL_PACK_STAGE} is not a prefix wxl was installed into "
            "with the WinUI layer (WXL_INSTALL and WXL_UI on).")
    endif()
endforeach()

file(STRINGS "${_wxl_version_file}" _wxl_line REGEX "^set\\(PACKAGE_VERSION \"[0-9.]+\"\\)$")
if(NOT _wxl_line MATCHES "\"([0-9.]+)\"")
    message(FATAL_ERROR "pack.cmake: no set(PACKAGE_VERSION \"...\") in ${_wxl_version_file}.")
endif()
set(WXL_PACK_VERSION "${CMAKE_MATCH_1}")

if(NOT WXL_PACK_COMMIT)
    find_package(Git QUIET)
    if(NOT Git_FOUND OR NOT EXISTS "${_wxl_tree}/.git")
        message(FATAL_ERROR
            "pack.cmake: no WXL_PACK_COMMIT, and ${_wxl_tree} is not a git checkout to read the "
            "commit of: the manifest names the commit the package is built from.")
    endif()
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${_wxl_tree}"
        OUTPUT_VARIABLE WXL_PACK_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY)
endif()
string(LENGTH "${WXL_PACK_COMMIT}" _wxl_length)
if(NOT WXL_PACK_COMMIT MATCHES "^[0-9a-f]+$" OR NOT _wxl_length EQUAL 40)
    message(FATAL_ERROR "pack.cmake: WXL_PACK_COMMIT is '${WXL_PACK_COMMIT}', not a full commit hash.")
endif()

# Every pin, held at the version the build restored ("[X]"), becomes a
# dependency on at least that version.
file(READ "${_wxl_pins_file}" _wxl_pins)
string(JSON _wxl_count LENGTH "${_wxl_pins}" packages)
set(WXL_PACK_DEPENDENCIES "")
set(_wxl_separator "")
math(EXPR _wxl_last "${_wxl_count} - 1")
foreach(_wxl_i RANGE ${_wxl_last})
    string(JSON _wxl_id GET "${_wxl_pins}" packages ${_wxl_i} id)
    string(JSON _wxl_version GET "${_wxl_pins}" packages ${_wxl_i} version)
    if(NOT _wxl_version MATCHES "^\\[([^],]+)\\]$")
        message(FATAL_ERROR
            "pack.cmake: ${_wxl_pins_file} holds ${_wxl_id} at '${_wxl_version}', not at the one "
            "version [X] the build restored.")
    endif()
    string(APPEND WXL_PACK_DEPENDENCIES
        "${_wxl_separator}        <dependency id=\"${_wxl_id}\" version=\"${CMAKE_MATCH_1}\" />")
    set(_wxl_separator "\n")
endforeach()

# The sources of the manifest's files in the native form: with forward slashes
# the wildcard of the prefix matches nothing, and dotnet pack writes a package
# of the manifest and the license alone without a word.
file(TO_NATIVE_PATH "${_wxl_tree}/LICENSE" WXL_PACK_LICENSE)
file(TO_NATIVE_PATH "${WXL_PACK_STAGE}" WXL_PACK_STAGE)
set(_wxl_nuspec "${WXL_PACK_OUTPUT}/WinXL.nuspec")
set(_wxl_package "${WXL_PACK_OUTPUT}/WinXL.${WXL_PACK_VERSION}.nupkg")
file(MAKE_DIRECTORY "${WXL_PACK_OUTPUT}")
file(REMOVE "${_wxl_package}")
configure_file("${CMAKE_CURRENT_LIST_DIR}/WinXL.nuspec.in" "${_wxl_nuspec}" @ONLY)

find_program(_wxl_dotnet dotnet REQUIRED)
execute_process(
    COMMAND "${_wxl_dotnet}" pack "${_wxl_nuspec}" --output "${WXL_PACK_OUTPUT}" --nologo
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    COMMAND_ERROR_IS_FATAL ANY)
if(NOT EXISTS "${_wxl_package}")
    message(FATAL_ERROR "pack.cmake: dotnet pack succeeded and wrote no ${_wxl_package}.")
endif()

# The package holds the prefix: its configuration is where find_package looks,
# and its MSBuild files are where NuGet imports them.
set(_wxl_check "${WXL_PACK_OUTPUT}/check")
set(_wxl_expected
    "build/native/lib/cmake/wxl/wxl-config.cmake"
    "build/native/WinXL.props"
    "build/native/WinXL.targets")
file(REMOVE_RECURSE "${_wxl_check}")
file(ARCHIVE_EXTRACT INPUT "${_wxl_package}" DESTINATION "${_wxl_check}" PATTERNS ${_wxl_expected})
foreach(_wxl_file IN LISTS _wxl_expected)
    if(NOT EXISTS "${_wxl_check}/${_wxl_file}")
        message(FATAL_ERROR
            "pack.cmake: ${_wxl_package} has no ${_wxl_file}: the prefix did not reach the package.")
    endif()
endforeach()
file(REMOVE_RECURSE "${_wxl_check}")

file(SIZE "${_wxl_package}" _wxl_size)
math(EXPR _wxl_size "${_wxl_size} / 1048576")
message(STATUS "WinXL ${WXL_PACK_VERSION} (${WXL_PACK_COMMIT}): ${_wxl_package}, ${_wxl_size} MB")
