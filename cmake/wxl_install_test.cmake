# A test of cmake/wxl_install.cmake: the tree installed into a prefix under
# WXL_TEST_DIR, and a project outside the tree configured and built against
# it. Run as a test (ctest -R "install-(core|ui)"), and by hand with the
# arguments the root passes to it (cmake/wxl_install.cmake):
#
#     cmake -DWXL_TEST_NAME=install-core -DWXL_TEST_DIR=<directory>
#           -DWXL_TEST_BUILD=<build tree> -DWXL_TEST_CONFIG=Debug
#           -DWXL_TEST_CONSUMER=<tests/install-consumer> ...
#           -P wxl_install_test.cmake
#
# The directory is emptied first. The sources of the consumer are copied into
# it, with WXL_TEST_PROFILE when one is given, so the project reaches nothing
# of the tree but the prefix. What is checked: the install of one
# configuration; find_package(wxl CONFIG REQUIRED) taking the package from the
# prefix, helpers included; the build of the consumer; and, with
# WXL_TEST_PROGRAM, the program run and printing WXL_TEST_EXPECTED.
#
# The consumer is given the compiler, the make program, the resource compiler
# and the manifest tool of the tree -- the link of an executable runs the last
# two -- and the runtime library of the tree, since an archive and its importer
# have to agree on it. The test runs in the developer environment all the
# same: the compiler and the linker of the consumer take the directories of
# the Windows SDK from its INCLUDE and LIB, and without them the link of the
# consumer's first test program stops on kernel32.lib.

foreach(_wxl_arg WXL_TEST_NAME WXL_TEST_DIR WXL_TEST_BUILD WXL_TEST_CONFIG WXL_TEST_CONSUMER
                 WXL_TEST_COMPILER WXL_TEST_MAKE_PROGRAM WXL_TEST_IMPORT_STD_UUID)
    if(NOT ${_wxl_arg})
        message(FATAL_ERROR "${WXL_TEST_NAME}: ${_wxl_arg} is not given.")
    endif()
endforeach()

set(_wxl_prefix "${WXL_TEST_DIR}/prefix")
set(_wxl_source "${WXL_TEST_DIR}/consumer")
set(_wxl_build "${WXL_TEST_DIR}/build")

file(REMOVE_RECURSE "${WXL_TEST_DIR}")
file(MAKE_DIRECTORY "${WXL_TEST_DIR}")
file(COPY "${WXL_TEST_CONSUMER}/" DESTINATION "${_wxl_source}")
if(WXL_TEST_PROFILE)
    file(COPY "${WXL_TEST_PROFILE}" DESTINATION "${_wxl_source}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${WXL_TEST_BUILD}"
            --config "${WXL_TEST_CONFIG}" --prefix "${_wxl_prefix}"
    COMMAND_ERROR_IS_FATAL ANY)

# A tool or a runtime library is passed only when the tree names one;
# otherwise both take CMake's default.
set(_wxl_tools "")
foreach(_wxl_pair IN ITEMS "RC_COMPILER;CMAKE_RC_COMPILER" "MT;CMAKE_MT"
                           "RUNTIME_LIBRARY;CMAKE_MSVC_RUNTIME_LIBRARY")
    list(GET _wxl_pair 0 _wxl_given)
    list(GET _wxl_pair 1 _wxl_variable)
    if(WXL_TEST_${_wxl_given})
        list(APPEND _wxl_tools "-D${_wxl_variable}=${WXL_TEST_${_wxl_given}}")
    endif()
endforeach()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${_wxl_source}" -B "${_wxl_build}" -G Ninja
            "-DCMAKE_BUILD_TYPE=${WXL_TEST_CONFIG}"
            "-DCMAKE_CXX_COMPILER=${WXL_TEST_COMPILER}"
            "-DCMAKE_MAKE_PROGRAM=${WXL_TEST_MAKE_PROGRAM}"
            "-DCMAKE_PREFIX_PATH=${_wxl_prefix}"
            "-DWXL_TEST_IMPORT_STD_UUID=${WXL_TEST_IMPORT_STD_UUID}"
            ${_wxl_tools}
    COMMAND_ERROR_IS_FATAL ANY)

# The package came from the prefix, and not from a build tree or a registry.
file(STRINGS "${_wxl_build}/CMakeCache.txt" _wxl_dir REGEX "^wxl_DIR:")
string(REGEX REPLACE "^wxl_DIR:[A-Z]+=" "" _wxl_dir "${_wxl_dir}")
cmake_path(IS_PREFIX _wxl_prefix "${_wxl_dir}" NORMALIZE _wxl_from_prefix)
if(NOT _wxl_from_prefix)
    message(FATAL_ERROR
        "${WXL_TEST_NAME}: find_package(wxl) took ${_wxl_dir}, not the prefix ${_wxl_prefix}.")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${_wxl_build}"
    COMMAND_ERROR_IS_FATAL ANY)

if(NOT WXL_TEST_PROGRAM)
    message(STATUS "${WXL_TEST_NAME}: ${_wxl_source} built against ${_wxl_prefix}")
    return()
endif()

execute_process(
    COMMAND "${_wxl_build}/${WXL_TEST_PROGRAM}"
    OUTPUT_VARIABLE _wxl_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)

if(NOT _wxl_output STREQUAL WXL_TEST_EXPECTED)
    message(FATAL_ERROR
        "${WXL_TEST_NAME}: the consumer printed\n  ${_wxl_output}\nexpected\n  ${WXL_TEST_EXPECTED}")
endif()
message(STATUS "${WXL_TEST_NAME}: ${_wxl_output}")
