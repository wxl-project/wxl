# A project outside the wxl tree that takes wxl from an installed prefix. The
# test install-core (cmake/wxl_install_test.cmake) copies it out of the tree,
# configures it with CMAKE_PREFIX_PATH naming the prefix, builds and runs it.
cmake_minimum_required(VERSION 4.3)

# The `import std;` gate opens before project(), and the test hands over the
# UUID of the CMake release that configures it.
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "${WXL_TEST_IMPORT_STD_UUID}")

project(wxl_install_consumer LANGUAGES CXX)

# The language of wxl, for the whole directory and before the package: a
# consumer builds the module interfaces of wxl with its own options.
add_compile_options(/permissive- /utf-8 /Zc:throwingNew)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

find_package(wxl CONFIG REQUIRED)

add_executable(install-consumer main.cpp)
target_link_libraries(install-consumer PRIVATE wxl::core)
