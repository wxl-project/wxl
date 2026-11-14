# A test of wxl_runtime_library (cmake/wxl_runtime_library.cmake): the CRT and
# the build of wxl's archives a configuration gets, read off the values of
# CMAKE_MSVC_RUNTIME_LIBRARY an application sets. Run as a test
# (ctest -R runtime-library), and by hand:
#
#     cmake -P wxl_runtime_library_test.cmake
#
# Each case runs in a cmake process of its own, since a stop ends the script it
# happens in. What is checked: the four runtime libraries, CMake's default, the
# forms the presets of wxl and of its applications write, a configuration that
# is no Debug and no Release; and the values the function cannot read, which
# stop.

cmake_minimum_required(VERSION 3.28)

include("${CMAKE_CURRENT_LIST_DIR}/wxl_runtime_library.cmake")

if(DEFINED WXL_TEST_CONFIG)
    wxl_runtime_library(crt "${WXL_TEST_CONFIG}")
    message(STATUS "${crt_RUNTIME} ${crt_VARIANT} ${crt_CONFIG}")
    return()
endif()

# _wxl_test_case(<value> <config> <expected>|STOP)
function(_wxl_test_case value config expected)
    execute_process(
        COMMAND "${CMAKE_COMMAND}" "-DCMAKE_MSVC_RUNTIME_LIBRARY=${value}"
                "-DWXL_TEST_CONFIG=${config}" -P "${CMAKE_CURRENT_FUNCTION_LIST_FILE}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE output)
    if(expected STREQUAL "STOP")
        if(result EQUAL 0)
            message(FATAL_ERROR "runtime-library: '${value}' in ${config} passed:\n${output}")
        endif()
        return()
    endif()
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "runtime-library: '${value}' in ${config} stopped:\n${output}")
    endif()
    string(FIND "${output}" "-- ${expected}\n" at)
    if(at EQUAL -1)
        message(FATAL_ERROR
            "runtime-library: '${value}' in ${config}: expected '${expected}', got\n${output}")
    endif()
endfunction()

# CMake's default, the value of x64-dynamic and of the samples.
foreach(value "" "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    _wxl_test_case("${value}" Debug "MultiThreadedDebugDLL md Debug")
    _wxl_test_case("${value}" Release "MultiThreadedDLL md Release")
    _wxl_test_case("${value}" RelWithDebInfo "MultiThreadedDLL md Release")
endforeach()

# x64-static.
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:Debug>" Debug "MultiThreadedDebug mt Debug")
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:Debug>" Release "MultiThreaded mt Release")

# The CRT in a DLL in Debug, the static one elsewhere.
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:DebugDLL>" Debug "MultiThreadedDebugDLL md Debug")
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:DebugDLL>" Release "MultiThreaded mt Release")
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:DebugDLL>" MinSizeRel "MultiThreaded mt Release")

# Plain values, the configuration compared without case, a list of
# configurations, $<IF> and nesting.
_wxl_test_case("MultiThreadedDLL" Debug "MultiThreadedDLL md Release")
_wxl_test_case("MultiThreaded$<$<CONFIG:debug>:Debug>DLL" Debug "MultiThreadedDebugDLL md Debug")
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug,Checked>:Debug>" Checked "MultiThreadedDebug mt Debug")
_wxl_test_case("$<IF:$<CONFIG:Debug>,MultiThreadedDebugDLL,MultiThreaded>" Release "MultiThreaded mt Release")
_wxl_test_case("$<IF:$<CONFIG:Debug>,MultiThreadedDebugDLL,MultiThreaded>" Debug "MultiThreadedDebugDLL md Debug")
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<CONFIG:Release>>:DLL>" Debug STOP)

# The configuration of a single-config generator without CMAKE_BUILD_TYPE.
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" "" "MultiThreadedDLL md Release")

# Not a runtime library.
_wxl_test_case("MultiThreadedDebugDll" Debug STOP)
_wxl_test_case("MultiThreaded$<$<CONFIG:Debug>:Debug>DLLx" Release STOP)
_wxl_test_case("$<$<CXX_COMPILER_ID:MSVC>:MultiThreadedDLL>" Release STOP)

message(STATUS "runtime-library: passed")
