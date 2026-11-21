# The test restored-packages-msbuild: the list of the restore that
# cmake/Write-RestoredPackages.ps1 writes for a .vcxproj (WinXL.targets) is the
# text cmake/wxl_restored_packages.cmake wrote from the same restore -- the
# restore of this build, <build>/wxl.packages:
#
#     cmake -DWXL_TEST_PACKAGES=<build>/wxl.packages -DWXL_TEST_DIR=<dir>
#           -P cmake/wxl_restored_packages_msbuild_test.cmake
#
# The script is handed what WinXL.targets hands it: the restore result, the
# version of the Windows SDK and the .winmd files of the platform projection,
# here the -in entries of the response file the configure read them from.

cmake_minimum_required(VERSION 3.28)

foreach(_wxl_arg WXL_TEST_PACKAGES WXL_TEST_DIR)
    if(NOT ${_wxl_arg})
        message(FATAL_ERROR "${_wxl_arg} is not given.")
    endif()
endforeach()

set(_wxl_expected "${WXL_TEST_PACKAGES}/restored-packages.json")
set(_wxl_response "${WXL_TEST_PACKAGES}/int/wxl.packages.vcxproj.cppwinrt_plat.rsp")
foreach(_wxl_file IN ITEMS "${_wxl_expected}" "${_wxl_response}")
    if(NOT EXISTS "${_wxl_file}")
        message(FATAL_ERROR "No ${_wxl_file}: the configure of the build restores the packages.")
    endif()
endforeach()

file(READ "${_wxl_expected}" _wxl_list)
string(JSON _wxl_sdk GET "${_wxl_list}" windowsSdk version)

file(READ "${_wxl_response}" _wxl_rest)
set(_wxl_metadata "")
while(_wxl_rest MATCHES "-in \"([^\"]+)\"")
    string(APPEND _wxl_metadata "${CMAKE_MATCH_1}\n")
    string(FIND "${_wxl_rest}" "${CMAKE_MATCH_0}" _wxl_at)
    string(LENGTH "${CMAKE_MATCH_0}" _wxl_length)
    math(EXPR _wxl_at "${_wxl_at} + ${_wxl_length}")
    string(SUBSTRING "${_wxl_rest}" ${_wxl_at} -1 _wxl_rest)
endwhile()

file(REMOVE_RECURSE "${WXL_TEST_DIR}")
file(MAKE_DIRECTORY "${WXL_TEST_DIR}")
file(WRITE "${WXL_TEST_DIR}/metadata.txt" "${_wxl_metadata}")
set(_wxl_actual "${WXL_TEST_DIR}/restored-packages.json")

find_program(_wxl_powershell NAMES powershell REQUIRED)
execute_process(
    COMMAND "${_wxl_powershell}" -NoProfile -ExecutionPolicy Bypass -File
            "${CMAKE_CURRENT_LIST_DIR}/Write-RestoredPackages.ps1"
            -Assets "${WXL_TEST_PACKAGES}/obj/project.assets.json"
            -WindowsSdk "${_wxl_sdk}"
            -Metadata "${WXL_TEST_DIR}/metadata.txt"
            -Out "${_wxl_actual}"
    RESULT_VARIABLE _wxl_result
    OUTPUT_VARIABLE _wxl_output
    ERROR_VARIABLE _wxl_output)
if(NOT _wxl_result EQUAL 0)
    message(FATAL_ERROR "Write-RestoredPackages.ps1 failed (${_wxl_result}):\n${_wxl_output}")
endif()

file(READ "${_wxl_actual}" _wxl_written)
if(NOT _wxl_written STREQUAL _wxl_list)
    message(FATAL_ERROR
        "Write-RestoredPackages.ps1 wrote ${_wxl_actual}, which is not the text of ${_wxl_expected} "
        "the configure wrote from the same restore.")
endif()

# A line <id>|<directory> for every package of the list.
string(JSON _wxl_count LENGTH "${_wxl_list}" packages)
string(REGEX MATCHALL "[^\r\n]+" _wxl_lines "${_wxl_output}")
list(LENGTH _wxl_lines _wxl_printed)
if(NOT _wxl_printed EQUAL _wxl_count)
    message(FATAL_ERROR
        "Write-RestoredPackages.ps1 printed ${_wxl_printed} lines for the ${_wxl_count} packages of "
        "the list:\n${_wxl_output}")
endif()
math(EXPR _wxl_last "${_wxl_count} - 1")
foreach(_wxl_i RANGE ${_wxl_last})
    string(JSON _wxl_id GET "${_wxl_list}" packages ${_wxl_i} id)
    string(JSON _wxl_dir GET "${_wxl_list}" packages ${_wxl_i} directory)
    list(GET _wxl_lines ${_wxl_i} _wxl_line)
    if(NOT _wxl_line STREQUAL "${_wxl_id}|${_wxl_dir}")
        message(FATAL_ERROR "Write-RestoredPackages.ps1 printed '${_wxl_line}' for ${_wxl_id} at ${_wxl_dir}.")
    endif()
endforeach()

message(STATUS "The list of Write-RestoredPackages.ps1 is the configure's: ${_wxl_count} packages.")
