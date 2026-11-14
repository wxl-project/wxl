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
#
# Two builds of the tree install into one prefix as well: one with the CRT in a
# DLL (x64-dynamic) and one with the static CRT (x64-static), which is how the
# package of a release carries both. Everything but the archives and the
# export is the same in both and is written over; the archives of a build go
# into lib/<variant> and its export into lib/cmake/wxl/<variant>, md for the
# CRT in a DLL and mt for the static one, read off CMAKE_MSVC_RUNTIME_LIBRARY
# (cmake/wxl_runtime_library.cmake). The configuration of the package gives
# every configuration of a project the archives of the build its CRT asks for.
# A build directory installs one variant, Debug with a debug CRT and Release
# with a release one: anything else stops the configure here.

include_guard(GLOBAL)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/wxl_runtime_library.cmake")

set(WXL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/wxl")
set(WXL_HELPERS_RELATIVE_DIR "helpers")

wxl_runtime_configurations(_wxl_configs)
set(WXL_INSTALL_VARIANT "")
foreach(_wxl_config IN LISTS _wxl_configs)
    if(_wxl_config STREQUAL "<none>")
        set(_wxl_config "")
    endif()
    wxl_runtime_library(_wxl_crt "${_wxl_config}")
    if(_wxl_config MATCHES "^(Debug|Release)$" AND NOT _wxl_crt_CONFIG STREQUAL _wxl_config)
        message(FATAL_ERROR
            "WXL_INSTALL: CMAKE_MSVC_RUNTIME_LIBRARY is '${CMAKE_MSVC_RUNTIME_LIBRARY}', which "
            "builds ${_wxl_config} with ${_wxl_crt_RUNTIME}: an installed wxl links its Debug "
            "archives for a debug CRT and its Release ones for a release CRT, so Debug is built "
            "with a debug CRT and Release with a release one.")
    endif()
    if(WXL_INSTALL_VARIANT STREQUAL "")
        set(WXL_INSTALL_VARIANT "${_wxl_crt_VARIANT}")
    elseif(NOT WXL_INSTALL_VARIANT STREQUAL _wxl_crt_VARIANT)
        message(FATAL_ERROR
            "WXL_INSTALL: CMAKE_MSVC_RUNTIME_LIBRARY is '${CMAKE_MSVC_RUNTIME_LIBRARY}', which "
            "takes the CRT in a DLL in one configuration and the static one in another. A build "
            "directory installs the archives of one of them; build the other in a directory of "
            "its own, as the presets x64-dynamic and x64-static do.")
    endif()
endforeach()
set(_wxl_lib_dir "${CMAKE_INSTALL_LIBDIR}/${WXL_INSTALL_VARIANT}")

install(TARGETS wxl.core
    EXPORT wxl-targets
    ARCHIVE DESTINATION ${_wxl_lib_dir}
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
        ARCHIVE DESTINATION ${_wxl_lib_dir}
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
        ARCHIVE DESTINATION ${_wxl_lib_dir}
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
# exported targets; without it the export is just archives. The export of a
# build is its variant's; wxl-config.cmake, one for both, picks it.
install(EXPORT wxl-targets
    FILE wxl-targets.cmake
    NAMESPACE wxl::
    DESTINATION "${WXL_CMAKE_DIR}/${WXL_INSTALL_VARIANT}"
    CXX_MODULES_DIRECTORY cxx-modules
)

# The module interface units of the export, IMPORTED_CXX_MODULES_<CONFIG>, are
# named by the files of the build -- the sources of wxl.core, the copy of fmt.cc
# in the build directory -- and not by the installed ones, when the rule that
# installs a file set is in another directory than its target, as the rules of
# this file are; CMake 4.5 names the installed files there as well. A path of
# the machine that built wxl is nowhere on the machine of an application, so
# after the export is installed the base directory of every module file set is
# replaced with its destination under ${_IMPORT_PREFIX}, which is what the
# export of a rule in the target's own directory says. A module still named by
# an absolute path afterwards stops the install.
set(_wxl_module_dirs "")
foreach(_wxl_pair IN ITEMS "wxl.core;modules;${CMAKE_INSTALL_INCLUDEDIR}/wxl"
                           "fmt.module;fmt;${CMAKE_INSTALL_INCLUDEDIR}/wxl/fmt")
    list(GET _wxl_pair 0 _wxl_target)
    list(GET _wxl_pair 1 _wxl_set)
    list(GET _wxl_pair 2 _wxl_destination)
    if(TARGET ${_wxl_target})
        get_target_property(_wxl_bases ${_wxl_target} CXX_MODULE_DIRS_${_wxl_set})
        foreach(_wxl_base IN LISTS _wxl_bases)
            list(APPEND _wxl_module_dirs "${_wxl_base}|${_wxl_destination}")
        endforeach()
    endif()
endforeach()
install(CODE "set(_WXL_INSTALL_MODULES_DIR \"${WXL_CMAKE_DIR}/${WXL_INSTALL_VARIANT}/cxx-modules\")
set(_WXL_INSTALL_MODULE_DIRS \"${_wxl_module_dirs}\")")
install(SCRIPT "${CMAKE_CURRENT_LIST_DIR}/wxl_install_modules.cmake")

# WinXL.props and WinXL.targets at the root of the prefix, for a .vcxproj that
# takes wxl from the package (cmake/wxl_msbuild.cmake).
include("${CMAKE_CURRENT_LIST_DIR}/wxl_msbuild.cmake")
wxl_install_msbuild()

# The helpers travel with the package, and wxl-config.cmake includes
# wxl_runtime_library.cmake, which picks the archives, and
# wxl_helpers.cmake: it and every file it includes, with the manifest template
# wxl_target_manifest() reads beside them. A file missing here stops
# find_package(wxl) in the consumer of the test install-core.
install(FILES
        "${CMAKE_CURRENT_LIST_DIR}/wxl_helpers.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/wxl_runtime_library.cmake"
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

# The version of the commit (WXL_VERSION, the root CMakeLists.txt), and any
# version of the same major one satisfies a request for an older one: the
# version find_package(wxl <version>) names is the least an application
# accepts.
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/wxl-config/wxl-config-version.cmake"
    VERSION ${WXL_VERSION}
    COMPATIBILITY SameMajorVersion
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
    VERSION ${WXL_VERSION}
    COMPATIBILITY SameMajorVersion
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
        "-DWXL_TEST_EXPECTED=wxl ${WXL_VERSION}: 9 code units")
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
