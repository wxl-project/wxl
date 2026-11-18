# /cmake/wxl_package_version.cmake
#
# WXL_PACKAGE_VERSION, the version of the wxl tree at its commit: the last
# release, which the tag v<major>.<minor>.<patch> names, with the number of
# commits since that tag as a fourth part. git describe --tags --match "v*"
# --long gives both:
#
#     v0.1.0-64-g5c6f44d4   ->  0.1.0.64
#     v0.2.0-0-g1a2b3c4d    ->  0.2.0      (the commit of the tag itself)
#
# A fourth part, not a prerelease label: find_package() takes numbers only, and
# NuGet orders 0.1.0 < 0.1.0.64 < 0.1.1. On main, which takes every pull
# request as one squashed commit, the count is the number of pull requests
# merged since the release. It needs the history down to the tag and the tag
# itself, which a shallow clone does not have.
#
# Two ways in:
#
#   include() by the root after project(). A WXL_PACKAGE_VERSION given with -D
#   wins. Without git, in a tree that is not a checkout of its own, or in a
#   checkout without the tag in its history -- a shallow clone -- the version
#   is PROJECT_VERSION, the last release the tree names: the configure says so
#   and goes on, as wxl is distributed as sources as well.
#
#   cmake -P cmake/wxl_package_version.cmake prints the version and nothing
#   else, and stops where git cannot give it: whoever asks for the version of a
#   commit this way -- the workflow that builds the package -- has no release
#   to fall back on.
#
# A tag that matches v* and is not v<major>.<minor>.<patch> stops both.

include_guard(GLOBAL)

# _wxl_package_version_from_git(<version> <reason> <tree>)
#
# The version of the checkout <tree>, or an empty one and the reason git cannot
# give it. Only a checkout of its own: a tree copied into another repository
# would otherwise get that repository's history.
function(_wxl_package_version_from_git version_variable reason_variable tree)
    set(${version_variable} "" PARENT_SCOPE)
    if(NOT EXISTS "${tree}/.git")
        set(${reason_variable} "${tree} is not a git checkout" PARENT_SCOPE)
        return()
    endif()
    find_package(Git QUIET)
    if(NOT Git_FOUND)
        set(${reason_variable} "git is not found" PARENT_SCOPE)
        return()
    endif()
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --match "v*" --long
        WORKING_DIRECTORY "${tree}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE described
        ERROR_VARIABLE error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE)
    if(NOT result EQUAL 0)
        set(${reason_variable}
            "git describe found no release tag v* in the history of ${tree} (${error})" PARENT_SCOPE)
        return()
    endif()
    if(NOT described MATCHES "^v([0-9]+\\.[0-9]+\\.[0-9]+)-([0-9]+)-g[0-9a-f]+$")
        message(FATAL_ERROR
            "git describe --tags --match \"v*\" --long gave '${described}' in ${tree}: the last "
            "release tag is not v<major>.<minor>.<patch>, and the version of the commit cannot be "
            "read off it.")
    endif()
    if(CMAKE_MATCH_2 EQUAL 0)
        set(${version_variable} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    else()
        set(${version_variable} "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}" PARENT_SCOPE)
    endif()
endfunction()

if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
    cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH _wxl_tree)
    _wxl_package_version_from_git(_wxl_version _wxl_reason "${_wxl_tree}")
    if(_wxl_version STREQUAL "")
        message(FATAL_ERROR "No version of the commit: ${_wxl_reason}.")
    endif()
    # The version alone on standard output, for the caller to read.
    execute_process(COMMAND "${CMAKE_COMMAND}" -E echo "${_wxl_version}")
elseif(NOT "$CACHE{WXL_PACKAGE_VERSION}" STREQUAL "")
    if(NOT "$CACHE{WXL_PACKAGE_VERSION}" MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+)?$")
        message(FATAL_ERROR
            "WXL_PACKAGE_VERSION is '$CACHE{WXL_PACKAGE_VERSION}': a version of wxl is "
            "<major>.<minor>.<patch> with an optional fourth number.")
    endif()
    set(WXL_PACKAGE_VERSION "$CACHE{WXL_PACKAGE_VERSION}")
    message(STATUS "wxl ${WXL_PACKAGE_VERSION} (WXL_PACKAGE_VERSION)")
else()
    cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH _wxl_tree)
    _wxl_package_version_from_git(WXL_PACKAGE_VERSION _wxl_reason "${_wxl_tree}")
    if(WXL_PACKAGE_VERSION STREQUAL "")
        set(WXL_PACKAGE_VERSION "${PROJECT_VERSION}")
        message(STATUS "wxl ${WXL_PACKAGE_VERSION}, the release the tree names: ${_wxl_reason}")
    else()
        message(STATUS "wxl ${WXL_PACKAGE_VERSION} (git describe)")
    endif()
    unset(_wxl_tree)
    unset(_wxl_reason)
endif()
