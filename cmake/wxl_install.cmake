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
# are made over the packages the configure restores, with paths into that
# restore. The installed configuration restores the packages the tree was built
# against -- the pins of share/wxl/packages.json, each at the version the restore
# of this build resolved (wxl_write_restored_pins) -- and makes them anew
# (cmake/wxl_ui_packages.cmake), before an application links wxl::ui.common.
#
# With it comes what wxl_add_ui_library needs to make the library of a profile
# in the application's build: the generator, exported as wxl::winui-srcgen, the
# generator's profiles and tables in share/wxl/profiles, the components --
# manifests, sources, headers -- in share/wxl/components, and the scripts of
# the profile path among the helpers. The configuration names the directories
# on the imported targets, where the tree names its own.
set(WXL_INSTALL_UI FALSE)
set(WXL_INSTALL_DATA_DIR "${CMAKE_INSTALL_DATADIR}/wxl")
if(TARGET wxl.ui.common)
    set(WXL_INSTALL_UI TRUE)
    install(TARGETS wxl.ui.common
        EXPORT wxl-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wxl/ui
    )
    install(TARGETS winui-srcgen
        EXPORT wxl-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    get_target_property(_wxl_profiles winui-srcgen WXL_PROFILES_DIR)
    get_target_property(_wxl_components wxl.ui.common WXL_UI_COMPONENTS_DIR)
    install(DIRECTORY "${_wxl_profiles}/" DESTINATION "${WXL_INSTALL_DATA_DIR}/profiles")
    install(DIRECTORY "${_wxl_components}/" DESTINATION "${WXL_INSTALL_DATA_DIR}/components")

    wxl_write_restored_pins("${CMAKE_CURRENT_BINARY_DIR}/wxl-config/packages.json")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/wxl-config/packages.json"
        DESTINATION "${WXL_INSTALL_DATA_DIR}")

    install(FILES
            "${CMAKE_CURRENT_LIST_DIR}/wxl_packages.cmake"
            "${CMAKE_CURRENT_LIST_DIR}/wxl_packages.vcxproj.in"
            "${CMAKE_CURRENT_LIST_DIR}/wxl_nuget_version.cmake"
            "${CMAKE_CURRENT_LIST_DIR}/wxl_restored_packages.cmake"
            "${CMAKE_CURRENT_LIST_DIR}/wxl_ui_packages.cmake"
            "${CMAKE_CURRENT_LIST_DIR}/wxl_ui_profile.cmake"
            "${CMAKE_CURRENT_LIST_DIR}/Read-ProfileComponents.ps1"
        DESTINATION "${WXL_CMAKE_DIR}/${WXL_HELPERS_RELATIVE_DIR}"
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
    PATH_VARS WXL_INSTALL_DATA_DIR
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
# alone. The consumer gets the compiler, the make program, the resource
# compiler, the manifest tool and the runtime library of the tree -- the last
# evaluated for that configuration -- since an archive and its importer have to
# agree on them.
#
#   install-core   tests/install-consumer: wxl.core and fmt built from the
#                  installed interfaces, the program run and printing the
#                  version of the package and the length of the text it
#                  formatted.
#   install-ui     tests/install-ui-consumer, with a copy of the tree's profile
#                  base beside it: wxl_add_ui_library of the package restores
#                  the packages, runs the installed generator and builds
#                  wxl::ui.base from the installed components, and an
#                  application links it. Linked, not run: a window needs the
#                  Windows App Runtime and a desktop.
function(_wxl_install_test name consumer)
    add_test(NAME ${name}
        COMMAND "${CMAKE_COMMAND}"
                "-DWXL_TEST_NAME=${name}"
                "-DWXL_TEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/${name}-test"
                "-DWXL_TEST_BUILD=${CMAKE_CURRENT_BINARY_DIR}"
                "-DWXL_TEST_CONFIG=$<CONFIG>"
                "-DWXL_TEST_CONSUMER=${PROJECT_SOURCE_DIR}/tests/${consumer}"
                "-DWXL_TEST_COMPILER=${CMAKE_CXX_COMPILER}"
                "-DWXL_TEST_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
                "-DWXL_TEST_RC_COMPILER=${CMAKE_RC_COMPILER}"
                "-DWXL_TEST_MT=${CMAKE_MT}"
                "-DWXL_TEST_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
                "-DWXL_TEST_IMPORT_STD_UUID=${WXL_IMPORT_STD_UUID}"
                ${ARGN}
                -P "${CMAKE_CURRENT_LIST_DIR}/wxl_install_test.cmake")
endfunction()

if(WXL_TESTS)
    _wxl_install_test(install-core install-consumer
        "-DWXL_TEST_PROGRAM=install-consumer.exe"
        "-DWXL_TEST_EXPECTED=wxl ${PROJECT_VERSION}: 9 code units")
    if(WXL_INSTALL_UI)
        get_target_property(_wxl_profiles winui-srcgen WXL_PROFILES_DIR)
        _wxl_install_test(install-ui install-ui-consumer
            "-DWXL_TEST_PROFILE=${_wxl_profiles}/base.winxl-profile")
        # The consumer generates and compiles a library of a profile, which takes
        # longer than the timeout CI gives a test. The install copies
        # winui-srcgen, which a build under ctest -j may link anew while it does
        # (docs.language): the lock the tests that run it take
        # (wxl.gen/tests/CMakeLists.txt).
        set_tests_properties(install-ui PROPERTIES
            TIMEOUT 1800
            RESOURCE_LOCK winui-srcgen)
    endif()
endif()
