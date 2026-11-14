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
#   the modules        the module interface units of wxl.core and fmt.module as
#                      installed, which a project compiles with its own options,
#                      as a project on CMake does;
#   the archives, the definitions and the options, of each configuration of
#                      the tree, which file(GENERATE) evaluates;
#   the system libraries the archives link.
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

# Characters MSBuild reads as syntax -- % $ @ ; ' -- or XML does -- < > & --
# cannot go into the files as they are; a value carrying one stops.
function(_wxl_msbuild_check_value what value)
    if(value MATCHES "[%$@;'<>&]")
        message(FATAL_ERROR
            "WXL_INSTALL: ${what} is '${value}', with a character MSBuild or XML reads as syntax "
            "(% $ @ ; ' < > &), which WinXL.props and WinXL.targets (cmake/wxl_msbuild.cmake) "
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
    string(REPLACE "/" "\\" include_dir "${CMAKE_INSTALL_INCLUDEDIR}/wxl")
    string(REPLACE "/" "\\" lib_dir "${CMAKE_INSTALL_LIBDIR}")

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

    _wxl_msbuild_system_libraries(libraries wxl.core)
    _wxl_msbuild_system_libraries(fmt_libraries fmt.module)
    list(APPEND libraries ${fmt_libraries})
    list(REMOVE_DUPLICATES libraries)

    # The names of the archives, and the definitions and the options wxl.core
    # gives the code that uses it, in each configuration.
    set(data "${CMAKE_CURRENT_BINARY_DIR}/wxl-msbuild")
    file(REMOVE_RECURSE "${data}")
    file(GENERATE OUTPUT "${data}/$<CONFIG>.cmake" CONTENT
"set(_wxl_archives [==[$<TARGET_LINKER_FILE_NAME:wxl.core>;$<TARGET_LINKER_FILE_NAME:fmt.module>]==])
set(_wxl_definitions [==[$<TARGET_PROPERTY:wxl.core,INTERFACE_COMPILE_DEFINITIONS>]==])
set(_wxl_options [==[$<TARGET_PROPERTY:wxl.core,INTERFACE_COMPILE_OPTIONS>]==])
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
set(WXL_MSBUILD_LIBRARIES [==[${libraries}]==])
set(WXL_MSBUILD_CORE_MODULES [==[${core_modules}]==])
set(WXL_MSBUILD_FMT_MODULES [==[${fmt_modules}]==])
set(WXL_MSBUILD_FMT_DEFINITIONS [==[${fmt_definitions}]==])
")

    install(CODE "set(_WXL_MSBUILD_DATA [==[${data}]==])")
    install(SCRIPT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/wxl_install_msbuild.cmake")
endfunction()
