# /cmake/wxl_runtime_library.cmake
#
# Which build of wxl's archives a configuration links: the build made with the
# CRT of that configuration, as CMAKE_MSVC_RUNTIME_LIBRARY gives it.
#
# An installed wxl may carry the archives of two builds of one tree, each in
# Debug and Release, Debug with the suffix d: lib/md, made with the CRT in a DLL
# (/MD, /MDd), and lib/mt, made with the static one (/MT, /MTd), with the export
# of each build in lib/cmake/wxl/md and lib/cmake/wxl/mt (cmake/wxl_install.cmake).
# A debug CRT and the iterator debugging of a Debug build go together, and the
# linker stops on a mix of either (LNK2038), so the CRT decides the
# configuration of wxl as well: Debug for a debug CRT, Release otherwise --
# RelWithDebInfo and MinSizeRel included.
#
# An application names its CRT with the standard variable, in its preset: for
# example MultiThreaded$<$<CONFIG:Debug>:DebugDLL> for the CRT in a DLL in Debug
# and the static one in Release. The value is a generator expression, and the
# location of an imported archive is a path, which CMake takes as it is: so the
# value is evaluated here, for every configuration of the project, and each
# configuration is given the archives of its build by name. The variable is
# read, and not the MSVC_RUNTIME_LIBRARY of a target: a target that sets
# another CRT for itself is stopped by the linker. The configurations are those
# the project has when it calls find_package(wxl).
#
#   wxl_runtime_library(<prefix> <config>)
#       sets, for the configuration <config> ("" for none):
#         <prefix>_RUNTIME  MultiThreaded, MultiThreadedDebug, MultiThreadedDLL
#                           or MultiThreadedDebugDLL;
#         <prefix>_VARIANT  md for a CRT in a DLL, mt for the static one;
#         <prefix>_CONFIG   Debug for a debug CRT, Release otherwise.
#       What is evaluated is the part of the generator expression language a
#       CRT is written in: $<CONFIG>, $<CONFIG:names>, $<0:...>, $<1:...> and
#       $<IF:0|1,...,...>, nested. Anything else stops the configure, as does a
#       value that is not one of the four. An empty or unset variable is CMake's
#       default, MultiThreaded$<$<CONFIG:Debug>:Debug>DLL.
#
#   wxl_runtime_configurations(<variable>)
#       sets <variable> to the configurations of the project:
#       CMAKE_CONFIGURATION_TYPES with a multi-config generator,
#       CMAKE_BUILD_TYPE otherwise, and <none> for the empty configuration of a
#       single-config generator without CMAKE_BUILD_TYPE.
#
#   wxl_import_targets(<directory>)
#       includes the targets of wxl from the export of one of the builds in
#       <directory>/<variant> and gives every configuration of the project the
#       archives of its build, lib/<variant>/<archive> of the prefix
#       (cmake/wxl-config.cmake.in). A package without that build stops the
#       configure, naming the configuration and its CRT.

include_guard(GLOBAL)

function(wxl_runtime_library prefix config)
    set(value "${CMAKE_MSVC_RUNTIME_LIBRARY}")
    if(value STREQUAL "")
        set(value "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
    set(given "${value}")
    string(TOUPPER "${config}" config_upper)

    string(REPLACE "$<CONFIG>" "${config}" value "${value}")
    while(value MATCHES "\\$<CONFIG:([^$<>]*)>")
        set(match "${CMAKE_MATCH_0}")
        string(REPLACE "," ";" names "${CMAKE_MATCH_1}")
        set(bit 0)
        foreach(name IN LISTS names)
            string(TOUPPER "${name}" name)
            if(name STREQUAL config_upper)
                set(bit 1)
            endif()
        endforeach()
        string(REPLACE "${match}" "${bit}" value "${value}")
    endwhile()

    # Innermost first: an expression with no other inside it.
    while(TRUE)
        if(value MATCHES "\\$<([01]):([^$<>]*)>")
            set(match "${CMAKE_MATCH_0}")
            set(result "")
            if(CMAKE_MATCH_1)
                set(result "${CMAKE_MATCH_2}")
            endif()
        elseif(value MATCHES "\\$<IF:([01]),([^$<>,]*),([^$<>,]*)>")
            set(match "${CMAKE_MATCH_0}")
            if(CMAKE_MATCH_1)
                set(result "${CMAKE_MATCH_2}")
            else()
                set(result "${CMAKE_MATCH_3}")
            endif()
        else()
            break()
        endif()
        string(REPLACE "${match}" "${result}" value "${value}")
    endwhile()

    if(value MATCHES "\\$<")
        message(FATAL_ERROR
            "CMAKE_MSVC_RUNTIME_LIBRARY is '${given}', which wxl cannot evaluate for the "
            "configuration '${config}': the archives of wxl a configuration links follow its CRT, "
            "and the CRT is read with $<CONFIG>, $<CONFIG:names>, $<0:...>, $<1:...> and "
            "$<IF:condition,...,...> only, for example MultiThreaded$<$<CONFIG:Debug>:DebugDLL>.")
    endif()
    if(NOT value MATCHES "^MultiThreaded(Debug)?(DLL)?$")
        message(FATAL_ERROR
            "CMAKE_MSVC_RUNTIME_LIBRARY is '${given}', which is '${value}' for the configuration "
            "'${config}': the archives of wxl are built with MultiThreaded, MultiThreadedDebug, "
            "MultiThreadedDLL or MultiThreadedDebugDLL, and with nothing else.")
    endif()

    set(${prefix}_RUNTIME "${value}" PARENT_SCOPE)
    if(CMAKE_MATCH_2)
        set(${prefix}_VARIANT md PARENT_SCOPE)
    else()
        set(${prefix}_VARIANT mt PARENT_SCOPE)
    endif()
    if(CMAKE_MATCH_1)
        set(${prefix}_CONFIG Debug PARENT_SCOPE)
    else()
        set(${prefix}_CONFIG Release PARENT_SCOPE)
    endif()
endfunction()

function(wxl_runtime_configurations variable)
    get_property(multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(multi)
        set(configs "${CMAKE_CONFIGURATION_TYPES}")
    else()
        set(configs "${CMAKE_BUILD_TYPE}")
    endif()
    if(configs STREQUAL "")
        set(configs "<none>")
    endif()
    set(${variable} "${configs}" PARENT_SCOPE)
endfunction()

function(wxl_import_targets directory)
    # Every configuration of the project with the key of its properties, the
    # suffix NOCONFIG for the empty one, as CMake names it.
    wxl_runtime_configurations(configs)
    set(count 0)
    foreach(config IN LISTS configs)
        if(config STREQUAL "<none>")
            set(config "")
        endif()
        if(config STREQUAL "")
            set(key NOCONFIG)
        else()
            string(TOUPPER "${config}" key)
        endif()
        wxl_runtime_library(crt "${config}")
        set(config_${count} "${config}")
        set(key_${count} "${key}")
        set(runtime_${count} "${crt_RUNTIME}")
        set(variant_${count} "${crt_VARIANT}")
        string(TOUPPER "${crt_CONFIG}" build_${count})
        math(EXPR count "${count} + 1")
    endforeach()
    math(EXPR last "${count} - 1")

    # The targets are the same whichever build exports them: the builds of a
    # package are builds of one tree, with the same targets, module interfaces
    # and archive names, apart in lib/<variant>. The export of the build the
    # first configuration links defines them.
    set(export "${directory}/${variant_0}/wxl-targets.cmake")
    if(NOT EXISTS "${export}")
        file(GLOB present LIST_DIRECTORIES true RELATIVE "${directory}" "${directory}/m[dt]")
        message(FATAL_ERROR
            "wxl at ${directory} has no build made with the CRT ${runtime_0}, which the "
            "configuration '${config_0}' links (CMAKE_MSVC_RUNTIME_LIBRARY is "
            "'${CMAKE_MSVC_RUNTIME_LIBRARY}'): there is no ${variant_0}/wxl-targets.cmake. Its "
            "builds: '${present}' -- md is made with the CRT in a DLL, mt with the static one.")
    endif()
    get_directory_property(before IMPORTED_TARGETS)
    include("${export}")
    get_directory_property(targets IMPORTED_TARGETS)
    if(before)
        list(REMOVE_ITEM targets ${before})
    endif()

    foreach(target IN LISTS targets)
        # What the export gives the target for each configuration of wxl, read
        # before a configuration of the project is written over it.
        foreach(build IN ITEMS DEBUG RELEASE)
            foreach(property IMPORTED_LOCATION IMPORTED_LINK_INTERFACE_LANGUAGES IMPORTED_CXX_MODULES)
                get_target_property(value ${target} ${property}_${build})
                set(${property}_${build} "${value}")
            endforeach()
        endforeach()

        get_target_property(type ${target} TYPE)
        foreach(i RANGE ${last})
            set(build "${build_${i}}")
            set(key "${key_${i}}")
            if(NOT IMPORTED_LOCATION_${build})
                message(FATAL_ERROR
                    "wxl at ${directory} has no ${build} build of ${target}, which the configuration "
                    "'${config_${i}}' links with the CRT ${runtime_${i}} "
                    "(CMAKE_MSVC_RUNTIME_LIBRARY is '${CMAKE_MSVC_RUNTIME_LIBRARY}').")
            endif()
            set(location "${IMPORTED_LOCATION_${build}}")
            if(type STREQUAL "STATIC_LIBRARY")
                # <prefix>/lib/<variant>/<archive>
                cmake_path(GET location FILENAME archive)
                cmake_path(GET location PARENT_PATH libdir)
                cmake_path(GET libdir PARENT_PATH libdir)
                set(location "${libdir}/${variant_${i}}/${archive}")
                if(NOT EXISTS "${location}")
                    message(FATAL_ERROR
                        "wxl at ${directory} has no build made with the CRT ${runtime_${i}}, which "
                        "the configuration '${config_${i}}' links (CMAKE_MSVC_RUNTIME_LIBRARY is "
                        "'${CMAKE_MSVC_RUNTIME_LIBRARY}'): there is no ${location}.")
                endif()
            endif()
            get_target_property(configurations ${target} IMPORTED_CONFIGURATIONS)
            if(NOT key IN_LIST configurations)
                set_property(TARGET ${target} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${key})
            endif()
            set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_${key} "${location}")
            foreach(property IMPORTED_LINK_INTERFACE_LANGUAGES IMPORTED_CXX_MODULES)
                if(${property}_${build})
                    set_property(TARGET ${target} PROPERTY ${property}_${key} ${${property}_${build}})
                endif()
            endforeach()
        endforeach()
    endforeach()
endfunction()
