# A test of cmake/wxl_install.cmake: the tree installed into a prefix under
# WXL_TEST_DIR, and the project tests/install-consumer configured, built and
# run against it. Run as a test (ctest -R install-core), and by hand with the
# arguments the root passes to it (cmake/wxl_install.cmake):
#
#     cmake -DWXL_TEST_DIR=<directory> -DWXL_TEST_BUILD=<build tree>
#           -DWXL_TEST_CONFIG=Debug -DWXL_TEST_CONSUMER=<tests/install-consumer> ...
#           -P wxl_install_test.cmake
#
# The directory is emptied first. The sources of the consumer are copied into
# it, so the project reaches nothing of the tree but the prefix. What is
# checked: the install of one configuration; find_package(wxl CONFIG REQUIRED)
# taking the package from the prefix, helpers included; the module wxl.core and
# fmt under it built from the installed interfaces; the archives of both
# linked; the program printing the version of the package and the length of
# the text it formatted.

foreach(_wxl_arg WXL_TEST_DIR WXL_TEST_BUILD WXL_TEST_CONFIG WXL_TEST_CONSUMER
                 WXL_TEST_COMPILER WXL_TEST_MAKE_PROGRAM WXL_TEST_IMPORT_STD_UUID
                 WXL_TEST_VERSION)
    if(NOT ${_wxl_arg})
        message(FATAL_ERROR "install-core: ${_wxl_arg} is not given.")
    endif()
endforeach()

set(_wxl_prefix "${WXL_TEST_DIR}/prefix")
set(_wxl_source "${WXL_TEST_DIR}/consumer")
set(_wxl_build "${WXL_TEST_DIR}/build")

file(REMOVE_RECURSE "${WXL_TEST_DIR}")
file(MAKE_DIRECTORY "${WXL_TEST_DIR}")
file(COPY "${WXL_TEST_CONSUMER}/" DESTINATION "${_wxl_source}")

execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${WXL_TEST_BUILD}"
            --config "${WXL_TEST_CONFIG}" --prefix "${_wxl_prefix}"
    COMMAND_ERROR_IS_FATAL ANY)

# The runtime library is passed only when the tree names one; otherwise both
# take CMake's default.
set(_wxl_runtime "")
if(WXL_TEST_RUNTIME_LIBRARY)
    set(_wxl_runtime "-DCMAKE_MSVC_RUNTIME_LIBRARY=${WXL_TEST_RUNTIME_LIBRARY}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${_wxl_source}" -B "${_wxl_build}" -G Ninja
            "-DCMAKE_BUILD_TYPE=${WXL_TEST_CONFIG}"
            "-DCMAKE_CXX_COMPILER=${WXL_TEST_COMPILER}"
            "-DCMAKE_MAKE_PROGRAM=${WXL_TEST_MAKE_PROGRAM}"
            "-DCMAKE_PREFIX_PATH=${_wxl_prefix}"
            "-DWXL_TEST_IMPORT_STD_UUID=${WXL_TEST_IMPORT_STD_UUID}"
            ${_wxl_runtime}
    COMMAND_ERROR_IS_FATAL ANY)

# The package came from the prefix, and not from a build tree or a registry.
file(STRINGS "${_wxl_build}/CMakeCache.txt" _wxl_dir REGEX "^wxl_DIR:")
string(REGEX REPLACE "^wxl_DIR:[A-Z]+=" "" _wxl_dir "${_wxl_dir}")
cmake_path(IS_PREFIX _wxl_prefix "${_wxl_dir}" NORMALIZE _wxl_from_prefix)
if(NOT _wxl_from_prefix)
    message(FATAL_ERROR "install-core: find_package(wxl) took ${_wxl_dir}, not the prefix ${_wxl_prefix}.")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${_wxl_build}"
    COMMAND_ERROR_IS_FATAL ANY)

execute_process(
    COMMAND "${_wxl_build}/install-consumer.exe"
    OUTPUT_VARIABLE _wxl_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)

set(_wxl_expected "wxl ${WXL_TEST_VERSION}: 9 code units")
if(NOT _wxl_output STREQUAL _wxl_expected)
    message(FATAL_ERROR "install-core: the consumer printed\n  ${_wxl_output}\nexpected\n  ${_wxl_expected}")
endif()
message(STATUS "install-core: ${_wxl_output}")
