# /tools/package/publish.cmake
#
# Publishes a NuGet package into a local feed, a directory NuGet restores from
# as from any other source:
#
#     cmake -DWXL_PUBLISH_PACKAGE=<package.nupkg> -DWXL_PUBLISH_FEED=<directory>
#           -P tools/package/publish.cmake
#
# The feed has the hierarchical layout of NuGet's local feeds,
# <feed>/<id>/<version>/ with the package, its manifest and its hash, the id
# and the version in lower case; an application names <directory> as a source
# in its nuget.config. The directory is made if it is not there.
#
# The package is installed into the feed by `dotnet package download` from the
# directory it lies in, the one source the command is given: the command writes
# the hierarchical layout into any directory, an empty one as well. `dotnet
# nuget push` does not: into a directory that is empty or not a hierarchical
# feed yet it copies the package flat and writes over a package of the same
# file name. The command came with the .NET SDK 10.0.200; global.json beside
# this script asks for that feature band or a later one, and dotnet reads it
# from the working directory, which is this one.
#
# A version is published once. A package of the same id and version that is in
# the feed already stops the script: `dotnet package download` would skip it
# without a word, and a restore would go on taking the package that is there.
# A new package gets a new version: the version of a commit
# (cmake/wxl_package_version.cmake) changes with every commit.

cmake_minimum_required(VERSION 3.28)

foreach(_wxl_arg WXL_PUBLISH_PACKAGE WXL_PUBLISH_FEED)
    if(NOT ${_wxl_arg})
        message(FATAL_ERROR "publish.cmake: ${_wxl_arg} is not given.")
    endif()
endforeach()
cmake_path(ABSOLUTE_PATH WXL_PUBLISH_PACKAGE NORMALIZE)
cmake_path(ABSOLUTE_PATH WXL_PUBLISH_FEED NORMALIZE)
string(REGEX REPLACE "/$" "" WXL_PUBLISH_FEED "${WXL_PUBLISH_FEED}")
if(NOT EXISTS "${WXL_PUBLISH_PACKAGE}" OR IS_DIRECTORY "${WXL_PUBLISH_PACKAGE}")
    message(FATAL_ERROR "publish.cmake: no package ${WXL_PUBLISH_PACKAGE}.")
endif()

# The id and the version are the manifest's, the ones NuGet reads: the file
# name of a package is not parsed back into them. The manifest is extracted
# beside the package, so that the feed holds only what NuGet writes.
cmake_path(GET WXL_PUBLISH_PACKAGE PARENT_PATH _wxl_source)
string(RANDOM LENGTH 12 _wxl_random)
set(_wxl_manifest_dir "${_wxl_source}/.publish-${_wxl_random}")
file(ARCHIVE_EXTRACT INPUT "${WXL_PUBLISH_PACKAGE}" DESTINATION "${_wxl_manifest_dir}" PATTERNS "*.nuspec")
file(GLOB _wxl_manifest "${_wxl_manifest_dir}/*.nuspec")
list(LENGTH _wxl_manifest _wxl_count)
if(NOT _wxl_count EQUAL 1)
    file(REMOVE_RECURSE "${_wxl_manifest_dir}")
    message(FATAL_ERROR "publish.cmake: ${WXL_PUBLISH_PACKAGE} has ${_wxl_count} manifests at its root, not one.")
endif()
file(READ "${_wxl_manifest}" _wxl_nuspec)
file(REMOVE_RECURSE "${_wxl_manifest_dir}")
foreach(_wxl_field id version)
    if(NOT _wxl_nuspec MATCHES "<${_wxl_field}>([^<]+)</${_wxl_field}>")
        message(FATAL_ERROR "publish.cmake: the manifest of ${WXL_PUBLISH_PACKAGE} has no <${_wxl_field}>.")
    endif()
    string(STRIP "${CMAKE_MATCH_1}" _wxl_${_wxl_field})
endforeach()
string(TOLOWER "${_wxl_id}" _wxl_id_lower)
string(TOLOWER "${_wxl_version}" _wxl_version_lower)
set(_wxl_published "${WXL_PUBLISH_FEED}/${_wxl_id_lower}/${_wxl_version_lower}")

if(EXISTS "${_wxl_published}")
    message(FATAL_ERROR
        "publish.cmake: ${_wxl_id} ${_wxl_version} is in the feed already, ${_wxl_published}. A "
        "version is published once; a restore takes the package that is there. Publish a "
        "package of a new version, or remove ${_wxl_published} and the copy of that version in "
        "the global packages folder of NuGet, which a restore has taken before.")
endif()

file(TO_NATIVE_PATH "${_wxl_source}" _wxl_source)
file(TO_NATIVE_PATH "${WXL_PUBLISH_FEED}" _wxl_feed)
file(MAKE_DIRECTORY "${WXL_PUBLISH_FEED}")

find_program(_wxl_dotnet dotnet REQUIRED)
execute_process(
    COMMAND "${_wxl_dotnet}" package download "${_wxl_id}@${_wxl_version}"
            --source "${_wxl_source}" --output "${_wxl_feed}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    COMMAND_ERROR_IS_FATAL ANY)

set(_wxl_nupkg "${_wxl_published}/${_wxl_id_lower}.${_wxl_version_lower}.nupkg")
foreach(_wxl_file IN ITEMS "${_wxl_nupkg}" "${_wxl_nupkg}.sha512" "${_wxl_published}/${_wxl_id_lower}.nuspec")
    if(NOT EXISTS "${_wxl_file}")
        message(FATAL_ERROR "publish.cmake: dotnet package download succeeded and wrote no ${_wxl_file}.")
    endif()
endforeach()
message(STATUS "${_wxl_id} ${_wxl_version}: ${_wxl_published}")
