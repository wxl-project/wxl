# A project outside the wxl tree that builds the library of a profile over
# wxl's extended from an installed prefix. The test install-extended
# (cmake/wxl_install_test.cmake) copies it out of the tree, configures it with
# CMAKE_PREFIX_PATH naming the prefix and builds it.
cmake_minimum_required(VERSION 4.5)

project(wxl_install_extended_consumer LANGUAGES CXX)

# The language of wxl, for the whole directory and before the package: a
# consumer builds the module interfaces of wxl with its own options.
add_compile_options(/permissive- /utf-8 /Zc:throwingNew)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

find_package(wxl CONFIG REQUIRED)

# The profile is the project's own and extends wxl's extended, which is not
# beside it: the generator and the configure find extended, and base under it,
# among the profiles of the package. extended selects the components bb, rsdn
# and code, so the library links wxl::bb, wxl::rsdn and wxl::highlight of the
# package and builds their modules from the installed interfaces.
wxl_add_ui_library(app PROFILE "${CMAKE_CURRENT_SOURCE_DIR}/app.winxl-profile")

wxl_add_executable(install-extended-consumer
    UI wxl::ui.app
    main.cpp)
