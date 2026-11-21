# /cmake/wxl_msbuild.cmake
#
# The MSBuild files of an installed wxl, WinXL.props and WinXL.targets at the
# root of the prefix: build/native of the package WinXL, where NuGet imports
# them into a .vcxproj that references the package. Included by
# cmake/wxl_install.cmake after the install rules of the targets:
#
#   wxl_install_msbuild()
#
# What the files say about wxl is read here from the tree, the way the tree
# builds wxl, and is not written a second time by hand:
#
#   the language       WXL_LANGUAGE_OPTIONS, and the option CMake compiles
#                      WXL_CXX_STANDARD with as the LanguageStandard of MSBuild
#                      (cmake/wxl_language.cmake);
#   the toolset        the version of the MSVC toolset of the compiler, from its
#                      path (VC/Tools/MSVC/<version>/bin): archives built by a
#                      toolset link only with that toolset or a newer one;
#   the modules        the module interface units of wxl.core, fmt.module and
#                      the libraries WXL_INSTALL_MODULE_LIBRARIES names
#                      (wxl.highlight, wxl.bb, wxl.rsdn) as installed, which a
#                      project compiles with its own options, as a project on
#                      CMake does;
#   the archives, the definitions and the options, of each configuration of
#                      the tree, which file(GENERATE) evaluates;
#   the system libraries the archives link;
#   the WinUI layer    with wxl.ui.common: its archive and link options, the
#                      generator and where the install puts the profiles, the
#                      components, the headers of wxl.ui.common and the scripts
#                      of the profile path; the files of the restored packages
#                      and the system libraries the layer stands on
#                      (_wxl_ui_package_files, cmake/wxl_ui_packages.cmake); how
#                      the sources of a library of a profile are compiled
#                      (_wxl_ui_profile_compile, cmake/wxl_ui_profile.cmake); the
#                      libraries a component may name, which WinXL.targets links
#                      already.
#
# The configurations are known only when CMake generates, after this file
# runs, so file(GENERATE) writes what each says into
# <build>/wxl-msbuild/<config>.cmake, and the install step
# cmake/wxl_install_msbuild.cmake reads them and writes the two files from
# WinXL.props.in and WinXL.targets.in beside this file. The files name no
# variant of the CRT: WinXL.targets picks lib/md or lib/mt of the prefix by the
# CRT a project compiles with, so the two builds of a package install the same
# files.

include_guard(GLOBAL)

# Characters MSBuild reads as syntax -- % $ @ ; ' -- or XML does -- < > & and
# the " that closes an attribute -- cannot go into the files as they are; a
# value carrying one stops.
function(_wxl_msbuild_check_value what value)
    if(value MATCHES "[%$@;'<>&\"]")
        message(FATAL_ERROR
            "WXL_INSTALL: ${what} is '${value}', with a character MSBuild or XML reads as syntax "
            "(% $ @ ; ' < > & \"), which WinXL.props and WinXL.targets (cmake/wxl_msbuild.cmake) "
            "would carry.")
    endif()
endfunction()

# The libraries <target> links that are no targets, each with .lib: the system
# libraries a link of its archive needs.
function(_wxl_msbuild_system_libraries out target)
    get_target_property(links ${target} LINK_LIBRARIES)
    if(NOT links)
        set(links "")
    endif()
    set(libraries "")
    foreach(link IN LISTS links)
        if(TARGET "${link}")
            continue()
        endif()
        if(link MATCHES "\\$<")
            message(FATAL_ERROR
                "WXL_INSTALL: ${target} links '${link}', a generator expression, which "
                "cmake/wxl_msbuild.cmake does not evaluate for the libraries of WinXL.targets.")
        endif()
        _wxl_msbuild_check_value("a library ${target} links" "${link}")
        if(NOT link MATCHES "\\.lib$")
            string(APPEND link ".lib")
        endif()
        list(APPEND libraries "${link}")
    endforeach()
    set(${out} "${libraries}" PARENT_SCOPE)
endfunction()

# The items <item> of the module interface units of the file set <set> of
# <target>, as installed under <destination> of the prefix, with the directory
# of each unit relative to its base directory: the build of a project puts the
# BMI of each unit into a directory of that name, since two units in different
# directories may share a file name (xml/node.ixx and html/node.ixx).
function(_wxl_msbuild_module_items out item target set destination)
    get_target_property(files ${target} CXX_MODULE_SET_${set})
    get_target_property(bases ${target} CXX_MODULE_DIRS_${set})
    set(items "")
    foreach(file IN LISTS files)
        set(relative "")
        foreach(base IN LISTS bases)
            cmake_path(IS_PREFIX base "${file}" NORMALIZE inside)
            if(inside)
                cmake_path(RELATIVE_PATH file BASE_DIRECTORY "${base}" OUTPUT_VARIABLE relative)
                break()
            endif()
        endforeach()
        if(relative STREQUAL "")
            message(FATAL_ERROR
                "WXL_INSTALL: the module ${file} of ${target} is under no base directory of its "
                "file set ${set} (${bases}).")
        endif()
        _wxl_msbuild_check_value("the module ${file}" "${relative}")
        cmake_path(GET relative PARENT_PATH directory)
        set(path "${destination}/${relative}")
        set(directory "${target}/${directory}/")
        string(REGEX REPLACE "/+" "/" directory "${directory}")
        string(REPLACE "/" "\\" path "${path}")
        string(REPLACE "/" "\\" directory "${directory}")
        string(APPEND items
            "    <${item} Include=\"$(WinXLRoot)${path}\" WinXLDirectory=\"${directory}\" />\n")
    endforeach()
    set(${out} "${items}" PARENT_SCOPE)
endfunction()

# A path of the prefix as WinXL.targets names it under $(WinXLRoot).
function(_wxl_msbuild_path out what path)
    _wxl_msbuild_check_value("${what}" "${path}")
    string(REPLACE "/" "\\" path "${path}")
    set(${out} "${path}" PARENT_SCOPE)
endfunction()

# Appends to <out> the item of a file of a restored package: <use> is link,
# runtime (put beside the program) or include (a directory of headers of the
# profile layer); <without> says what fails without the file.
function(_wxl_msbuild_package_file out use package path without)
    _wxl_msbuild_check_value("the package ${package}" "${package}")
    _wxl_msbuild_path(path "a file of ${package}" "${path}")
    _wxl_msbuild_check_value("what fails without a file of ${package}" "${without}")
    set(${out} "${${out}}    <WinXLPackageFile Include=\"${package}\" Path=\"${path}\" Use=\"${use}\" Without=\"${without}\" />\n"
        PARENT_SCOPE)
endfunction()

# What WinXL.targets needs of the WinUI layer to build a program on the library
# of a profile, as <prefix>_<NAME> in the caller's scope; <prefix>_TRUE is true.
# <component_libraries> are the libraries a component may name.
function(_wxl_msbuild_ui prefix component_libraries)
    _wxl_msbuild_path(profiles "the profiles directory" "${WXL_INSTALL_DATA_DIR}/profiles")
    _wxl_msbuild_path(components "the components directory" "${WXL_INSTALL_DATA_DIR}/components")
    _wxl_msbuild_path(include_dir "the include directory of wxl.ui.common" "${WXL_INSTALL_UI_INCLUDE_DIR}")
    _wxl_msbuild_path(helpers "the helpers directory" "${WXL_CMAKE_DIR}/${WXL_HELPERS_RELATIVE_DIR}")

    get_target_property(link_options wxl.ui.common INTERFACE_LINK_OPTIONS)
    if(NOT link_options)
        set(link_options "")
    endif()
    foreach(option IN LISTS link_options)
        if(option MATCHES "\\$<")
            message(FATAL_ERROR
                "WXL_INSTALL: wxl.ui.common gives the link '${option}', a generator expression, which "
                "cmake/wxl_msbuild.cmake does not evaluate.")
        endif()
        _wxl_msbuild_check_value("a link option of wxl.ui.common" "${option}")
    endforeach()
    list(JOIN link_options " " link_options)

    # The libraries wxl.ui.common links itself, and those of the profile layer.
    _wxl_ui_package_files(files)
    _wxl_msbuild_system_libraries(libraries wxl.ui.common)
    foreach(library IN LISTS files_LIBRARIES)
        _wxl_msbuild_check_value("a library of the profile layer" "${library}")
    endforeach()
    list(APPEND libraries ${files_LIBRARIES})
    list(REMOVE_DUPLICATES libraries)

    # The files of the restored packages, each an item WinXL.targets finds in
    # the restore of the project: to link, to put beside the program, or a
    # directory of headers of the profile layer.
    set(package_files "")
    list(GET files_BOOTSTRAP 0 package)
    list(GET files_BOOTSTRAP 1 dll)
    list(GET files_BOOTSTRAP 2 implib)
    set(without "applications built on wxl will fail to start")
    _wxl_msbuild_package_file(package_files link ${package} "${implib}" "${without}")
    _wxl_msbuild_package_file(package_files runtime ${package} "${dll}" "${without}")
    list(LENGTH files_RUNTIME count)
    math(EXPR last "${count} - 1")
    foreach(i RANGE 0 ${last} 3)
        math(EXPR j "${i} + 1")
        math(EXPR k "${i} + 2")
        list(GET files_RUNTIME ${i} package)
        list(GET files_RUNTIME ${j} path)
        list(GET files_RUNTIME ${k} without)
        _wxl_msbuild_package_file(package_files runtime ${package} "${path}" "${without}")
    endforeach()
    list(LENGTH files_INCLUDES count)
    math(EXPR last "${count} - 1")
    foreach(i RANGE 0 ${last} 2)
        math(EXPR j "${i} + 1")
        list(GET files_INCLUDES ${i} package)
        list(GET files_INCLUDES ${j} path)
        _wxl_msbuild_package_file(package_files include ${package} "${path}" "")
    endforeach()

    # The parts of the wrappers are named by two digits, 00 to 99, as
    # wxl_generate_wrappers names them.
    _wxl_ui_profile_compile(compile)
    set(part_numbers "")
    foreach(part RANGE 99)
        if(part LESS 10)
            set(part "0${part}")
        endif()
        list(APPEND part_numbers "${part}")
    endforeach()
    foreach(value IN LISTS compile_DEFINITIONS compile_PART_OPTIONS compile_SURFACE_DEFINITIONS
                           component_libraries)
        _wxl_msbuild_check_value("a setting of a library of a profile" "${value}")
    endforeach()
    list(JOIN compile_PART_OPTIONS " " part_options)

    set(${prefix}_TRUE true PARENT_SCOPE)
    set(${prefix}_PROFILES "${profiles}" PARENT_SCOPE)
    set(${prefix}_COMPONENTS "${components}" PARENT_SCOPE)
    set(${prefix}_INCLUDE_DIR "${include_dir}" PARENT_SCOPE)
    set(${prefix}_HELPERS "${helpers}" PARENT_SCOPE)
    set(${prefix}_LINK_OPTIONS "${link_options}" PARENT_SCOPE)
    set(${prefix}_LIBRARIES "${libraries}" PARENT_SCOPE)
    set(${prefix}_PACKAGE_FILES "${package_files}" PARENT_SCOPE)
    set(${prefix}_PARTS "${compile_PARTS}" PARENT_SCOPE)
    set(${prefix}_PART_NUMBERS "${part_numbers}" PARENT_SCOPE)
    set(${prefix}_DEFINITIONS "${compile_DEFINITIONS}" PARENT_SCOPE)
    set(${prefix}_PART_OPTIONS "${part_options}" PARENT_SCOPE)
    set(${prefix}_SURFACE_DEFINITIONS "${compile_SURFACE_DEFINITIONS}" PARENT_SCOPE)
    set(${prefix}_COMPONENT_LIBRARIES "${component_libraries}" PARENT_SCOPE)
endfunction()

function(wxl_install_msbuild)
    if(NOT TARGET fmt.module)
        message(FATAL_ERROR
            "WXL_INSTALL: no target fmt.module. wxl.core imports fmt, and WinXL.targets builds the "
            "module of fmt from the sources wxl installs, which only the fmt.module of "
            "wxl.core/src carries.")
    endif()

    set(option "${CMAKE_CXX${WXL_CXX_STANDARD}_STANDARD_COMPILE_OPTION}")
    if(NOT option MATCHES "^[-/]std:c\\+\\+([a-z0-9]+)$")
        message(FATAL_ERROR
            "WXL_INSTALL: CMake compiles C++${WXL_CXX_STANDARD} with '${option}', which is not "
            "/std:c++<level>, the LanguageStandard stdcpp<level> of MSBuild.")
    endif()
    set(standard "stdcpp${CMAKE_MATCH_1}")

    # Each option of the language, and the check of WinXL.targets that every C++
    # source still has it.
    set(checks "")
    foreach(language_option IN LISTS WXL_LANGUAGE_OPTIONS)
        _wxl_msbuild_check_value("the language option" "${language_option}")
        string(APPEND checks
            "      <_WinXLWrongLanguage Include=\"@(_WinXLCpp)\" Condition=\"!$([System.String]::new('%(_WinXLCpp.AdditionalOptions)').Contains('${language_option}'))\">\n"
            "        <WinXLWhat>no ${language_option}</WinXLWhat>\n"
            "      </_WinXLWrongLanguage>\n")
    endforeach()
    list(JOIN WXL_LANGUAGE_OPTIONS " " language_options)

    if(NOT CMAKE_CXX_COMPILER MATCHES "/VC/Tools/MSVC/([0-9]+\\.[0-9]+\\.[0-9]+)/bin/")
        message(FATAL_ERROR
            "WXL_INSTALL: the compiler ${CMAKE_CXX_COMPILER} is not in VC/Tools/MSVC/<version>/bin "
            "of a Visual Studio installation, from where WinXL.targets takes the toolset the "
            "archives are built with.")
    endif()
    set(tools_version "${CMAKE_MATCH_1}")

    # Where cmake/wxl_install.cmake installs the module units.
    _wxl_msbuild_module_items(core_modules WinXLCoreModule wxl.core modules
        "${CMAKE_INSTALL_INCLUDEDIR}/wxl")
    _wxl_msbuild_module_items(fmt_modules WinXLFmtModule fmt.module fmt
        "${CMAKE_INSTALL_INCLUDEDIR}/wxl/fmt")
    set(library_modules "")
    set(library_archives "")
    set(library_targets "")
    foreach(library IN LISTS WXL_INSTALL_MODULE_LIBRARIES)
        _wxl_msbuild_module_items(items WinXLLibraryModule wxl.${library} modules
            "${CMAKE_INSTALL_INCLUDEDIR}/wxl/${library}")
        string(APPEND library_modules "${items}")
        string(APPEND library_archives ";$<TARGET_LINKER_FILE_NAME:wxl.${library}>")
        list(APPEND library_targets wxl.${library})
    endforeach()
    string(REPLACE "/" "\\" include_dir "${CMAKE_INSTALL_INCLUDEDIR}/wxl")
    string(REPLACE "/" "\\" lib_dir "${CMAKE_INSTALL_LIBDIR}")
    # The template of the manifest of an executable, installed among the
    # helpers (cmake/wxl_install.cmake), which wxl_target_manifest() reads too.
    string(REPLACE "/" "\\" manifest "${WXL_CMAKE_DIR}/${WXL_HELPERS_RELATIVE_DIR}/wxl_app.manifest.in")

    # The definitions of the unit of fmt are fmt.module's own, PRIVATE and the
    # same in every configuration (wxl.core/src/CMakeLists.txt).
    get_target_property(fmt_definitions fmt.module COMPILE_DEFINITIONS)
    foreach(definition IN LISTS fmt_definitions)
        if(definition MATCHES "\\$<")
            message(FATAL_ERROR
                "WXL_INSTALL: fmt.module defines '${definition}', a generator expression, which "
                "cmake/wxl_msbuild.cmake does not evaluate.")
        endif()
        _wxl_msbuild_check_value("a definition of fmt.module" "${definition}")
    endforeach()

    set(libraries "")
    foreach(target IN ITEMS wxl.core fmt.module ${library_targets})
        _wxl_msbuild_system_libraries(target_libraries ${target})
        list(APPEND libraries ${target_libraries})
    endforeach()
    list(REMOVE_DUPLICATES libraries)

    # The files give every source the definitions and the options of wxl.core,
    # and nothing of the libraries beside it: one of them that gives the code
    # that uses it anything wxl.core does not give stops here.
    get_target_property(core_options wxl.core INTERFACE_COMPILE_OPTIONS)
    if(NOT core_options)
        set(core_options "")
    endif()
    foreach(target IN LISTS library_targets)
        get_target_property(definitions ${target} INTERFACE_COMPILE_DEFINITIONS)
        get_target_property(options ${target} INTERFACE_COMPILE_OPTIONS)
        if(NOT options)
            set(options "")
        endif()
        list(REMOVE_ITEM options ${core_options})
        if(definitions OR options)
            message(FATAL_ERROR
                "WXL_INSTALL: ${target} gives the code that uses it the definitions '${definitions}' "
                "and the options '${options}', which wxl.core does not, and WinXL.props and "
                "WinXL.targets (cmake/wxl_msbuild.cmake) give a project only those of wxl.core.")
        endif()
    endforeach()

    # The libraries a component of a profile may name in its manifest: those
    # whose archives the files link to every program, as the export names them.
    set(component_libraries "")
    foreach(target IN ITEMS wxl.core ${library_targets})
        get_target_property(name ${target} EXPORT_NAME)
        if(NOT name)
            set(name ${target})
        endif()
        list(APPEND component_libraries "wxl::${name}")
    endforeach()

    if(TARGET wxl.ui.common)
        _wxl_msbuild_ui(ui "${component_libraries}")
        set(ui_archive "$<TARGET_LINKER_FILE_NAME:wxl.ui.common>")
        set(generator "${CMAKE_INSTALL_BINDIR}/$<TARGET_FILE_NAME:winui-srcgen>")
    else()
        set(ui_TRUE false)
        set(ui_archive "")
        set(generator "")
    endif()

    # The names of the archives, and the definitions and the options wxl.core
    # gives the code that uses it, in each configuration.
    set(data "${CMAKE_CURRENT_BINARY_DIR}/wxl-msbuild")
    file(REMOVE_RECURSE "${data}")
    file(GENERATE OUTPUT "${data}/$<CONFIG>.cmake" CONTENT
"set(_wxl_archives [==[$<TARGET_LINKER_FILE_NAME:wxl.core>;$<TARGET_LINKER_FILE_NAME:fmt.module>${library_archives}]==])
set(_wxl_definitions [==[$<TARGET_PROPERTY:wxl.core,INTERFACE_COMPILE_DEFINITIONS>]==])
set(_wxl_options [==[$<TARGET_PROPERTY:wxl.core,INTERFACE_COMPILE_OPTIONS>]==])
set(_wxl_ui_archives [==[${ui_archive}]==])
set(_wxl_generator [==[${generator}]==])
")

    # The rest is the same in every configuration.
    file(WRITE "${data}/common.cmake"
"set(_WXL_MSBUILD_TEMPLATES [==[${CMAKE_CURRENT_FUNCTION_LIST_DIR}]==])
set(WXL_MSBUILD_VERSION [==[${WXL_VERSION}]==])
set(WXL_MSBUILD_STANDARD [==[${standard}]==])
set(WXL_MSBUILD_LANGUAGE_OPTIONS [==[${language_options}]==])
set(WXL_MSBUILD_LANGUAGE_CHECKS [==[${checks}]==])
set(WXL_MSBUILD_TOOLS_VERSION [==[${tools_version}]==])
set(WXL_MSBUILD_INCLUDE_DIR [==[${include_dir}]==])
set(WXL_MSBUILD_LIB_DIR [==[${lib_dir}]==])
set(WXL_MSBUILD_MANIFEST [==[${manifest}]==])
set(WXL_MSBUILD_APP_NAME [==[@WXL_APP_NAME@]==])
set(WXL_MSBUILD_LIBRARIES [==[${libraries}]==])
set(WXL_MSBUILD_CORE_MODULES [==[${core_modules}]==])
set(WXL_MSBUILD_FMT_MODULES [==[${fmt_modules}]==])
set(WXL_MSBUILD_LIBRARY_MODULES [==[${library_modules}]==])
set(WXL_MSBUILD_FMT_DEFINITIONS [==[${fmt_definitions}]==])
set(WXL_MSBUILD_UI [==[${ui_TRUE}]==])
set(WXL_MSBUILD_UI_PROFILES [==[${ui_PROFILES}]==])
set(WXL_MSBUILD_UI_COMPONENTS [==[${ui_COMPONENTS}]==])
set(WXL_MSBUILD_UI_INCLUDE_DIR [==[${ui_INCLUDE_DIR}]==])
set(WXL_MSBUILD_UI_HELPERS [==[${ui_HELPERS}]==])
set(WXL_MSBUILD_UI_LINK_OPTIONS [==[${ui_LINK_OPTIONS}]==])
set(WXL_MSBUILD_UI_LIBRARIES [==[${ui_LIBRARIES}]==])
set(WXL_MSBUILD_UI_PACKAGE_FILES [==[${ui_PACKAGE_FILES}]==])
set(WXL_MSBUILD_UI_PARTS [==[${ui_PARTS}]==])
set(WXL_MSBUILD_UI_PART_NUMBERS [==[${ui_PART_NUMBERS}]==])
set(WXL_MSBUILD_UI_DEFINITIONS [==[${ui_DEFINITIONS}]==])
set(WXL_MSBUILD_UI_PART_OPTIONS [==[${ui_PART_OPTIONS}]==])
set(WXL_MSBUILD_UI_SURFACE_DEFINITIONS [==[${ui_SURFACE_DEFINITIONS}]==])
set(WXL_MSBUILD_UI_COMPONENT_LIBRARIES [==[${ui_COMPONENT_LIBRARIES}]==])
")

    install(CODE "set(_WXL_MSBUILD_DATA [==[${data}]==])")
    install(SCRIPT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/wxl_install_msbuild.cmake")
endfunction()
