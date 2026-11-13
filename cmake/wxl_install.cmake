# /cmake/wxl_install.cmake
#
# The install and export rules of wxl, for find_package(wxl CONFIG) against an
# installed prefix. Included by the root under WXL_INSTALL, after the
# directories that make the targets it installs.
#
# What ships: the archives of wxl.core, of the fmt module it imports and of
# wxl.ui.common; the module *interface sources* of wxl.core and fmt with the
# headers they include; the headers of wxl.ui.common; the CMake glue. Built
# module interfaces (.ifc) are deliberately absent -- they are tied to the
# exact compiler build and flags, so CMake never installs them and the consumer
# compiles the interfaces once in its own build tree, with the definitions and
# include directories the export records for each module target
# (IMPORTED_CXX_MODULES_*).
#
# One export set, wxl-targets: the package is one, and there are no components
# to ask for. Debug and Release of one build install into the same prefix, the
# archives of Debug with the suffix d.

include_guard(GLOBAL)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(WXL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/wxl")
set(WXL_HELPERS_RELATIVE_DIR "helpers")

install(TARGETS wxl.core
    EXPORT wxl-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET modules DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wxl
    FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wxl
)

# wxl.core links fmt::module PUBLIC, so an export of wxl.core has to carry it:
# the fmt.module wxl.core/src makes travels in the same export set, and its
# module source, fmt's headers and fmt's license go into include/wxl/fmt,
# beside wxl's own fmt interfaces. <fmt/...> then resolves through include/wxl,
# the directory a consumer already has from wxl::core. The module unit is not
# compiled against wxl::core, so the same directory is the install half of its
# own include directory, set here next to the rule that puts the headers
# there; a consumer builds the BMI of fmt with it. A fmt::module the project
# brought itself is the project's to export.
if(TARGET fmt.module)
    include(FetchContent)
    FetchContent_GetProperties(fmt)
    set(_wxl_include_dir "${CMAKE_INSTALL_INCLUDEDIR}/wxl")
    set(_wxl_fmt_dir "${_wxl_include_dir}/fmt")

    target_include_directories(fmt.module PRIVATE $<INSTALL_INTERFACE:${_wxl_include_dir}>)

    install(TARGETS fmt.module
        EXPORT wxl-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILE_SET fmt DESTINATION ${_wxl_fmt_dir}
    )
    install(DIRECTORY "${fmt_SOURCE_DIR}/include/fmt/" DESTINATION "${_wxl_fmt_dir}")
    install(FILES "${fmt_SOURCE_DIR}/LICENSE" DESTINATION "${_wxl_fmt_dir}")
endif()

# The export names what wxl.ui.common links over the NuGet packages -- the
# targets Microsoft::CppWinRT and wxl.winappsdk.bootstrap -- and does not carry
# them, nor the properties WXL_RUNTIME_FILES and WXL_UI_PROFILE_*: all of them
# are made over the packages the configure of the tree restores, with paths into
# that restore. The installed configuration does not make them, so an
# application that links wxl::ui.common from the prefix has to have them first.
if(TARGET wxl.ui.common)
    install(TARGETS wxl.ui.common
        EXPORT wxl-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wxl/ui
    )
endif()

# CXX_MODULES_DIRECTORY is what carries the C++ module information of the
# exported targets; without it the export is just archives.
install(EXPORT wxl-targets
    FILE wxl-targets.cmake
    NAMESPACE wxl::
    DESTINATION "${WXL_CMAKE_DIR}"
    CXX_MODULES_DIRECTORY cxx-modules
)

# The helpers travel with the package, and wxl-config.cmake includes
# wxl_helpers.cmake: it and every file it includes, with the manifest template
# wxl_target_manifest() reads beside them. A file missing here stops
# find_package(wxl) in the consumer of the test install-core.
install(FILES
        "${CMAKE_CURRENT_LIST_DIR}/wxl_helpers.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_add_module_library.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_target_folders.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_add_gtest.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_app_icon.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_target_assets.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_add_executable.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_app.manifest.in"
    DESTINATION "${WXL_CMAKE_DIR}/${WXL_HELPERS_RELATIVE_DIR}"
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/wxl-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/wxl-config/wxl-config.cmake"
    INSTALL_DESTINATION "${WXL_CMAKE_DIR}"
)

# ExactVersion while the library is pre-1.0: nothing here is stable enough
# for a consumer to assume compatibility across versions yet.
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/wxl-config/wxl-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/wxl-config/wxl-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/wxl-config/wxl-config-version.cmake"
    DESTINATION "${WXL_CMAKE_DIR}"
)

# Build-tree consumption: find_package(wxl CONFIG PATHS "<wxl-build-dir>")
# works against an un-installed build, which is handy for local debugging of
# a consumer without an install step.
set(WXL_SOURCE_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
export(EXPORT wxl-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/wxl-targets.cmake"
    NAMESPACE wxl::
    CXX_MODULES_DIRECTORY cxx-modules
)
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/wxl-build-tree-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/wxl-config.cmake"
    @ONLY
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/wxl-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

# The tree installed into a directory of the test's own, and a project outside
# the tree built against it (cmake/wxl_install_test.cmake), in the
# configuration ctest tests: the build has the archives of that configuration
# alone. The consumer gets the compiler, the make program and the runtime
# library of the tree -- the last evaluated for that configuration -- since an
# archive and its importer have to agree on them.
if(WXL_TESTS)
    add_test(NAME install-core
        COMMAND "${CMAKE_COMMAND}"
                "-DWXL_TEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/install-core-test"
                "-DWXL_TEST_BUILD=${CMAKE_CURRENT_BINARY_DIR}"
                "-DWXL_TEST_CONFIG=$<CONFIG>"
                "-DWXL_TEST_CONSUMER=${PROJECT_SOURCE_DIR}/tests/install-consumer"
                "-DWXL_TEST_COMPILER=${CMAKE_CXX_COMPILER}"
                "-DWXL_TEST_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
                "-DWXL_TEST_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
                "-DWXL_TEST_IMPORT_STD_UUID=${WXL_IMPORT_STD_UUID}"
                "-DWXL_TEST_VERSION=${PROJECT_VERSION}"
                -P "${CMAKE_CURRENT_LIST_DIR}/wxl_install_test.cmake")
endif()
