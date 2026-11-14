# /cmake/wxl_install_msbuild.cmake
#
# A step of the install of wxl (cmake/wxl_msbuild.cmake): writes WinXL.props
# and WinXL.targets into the root of the prefix from WinXL.props.in and
# WinXL.targets.in. Given by the step before it: _WXL_MSBUILD_DATA, the
# directory the configure wrote what the files say into -- common.cmake, the
# same in every configuration, and Debug.cmake and Release.cmake, what each
# configuration of the tree says.
#
# Both configurations are read whichever of them is installed: the files name
# the archives of both, and a project picks one by its CRT.

include("${_WXL_MSBUILD_DATA}/common.cmake")

foreach(_wxl_build IN ITEMS Debug Release)
    set(_wxl_file "${_WXL_MSBUILD_DATA}/${_wxl_build}.cmake")
    if(NOT EXISTS "${_wxl_file}")
        message(FATAL_ERROR
            "No ${_wxl_file}: WinXL.targets names the archives of Debug and Release, and the "
            "tree is configured without ${_wxl_build} (CMAKE_CONFIGURATION_TYPES).")
    endif()
    include("${_wxl_file}")
    string(TOUPPER "${_wxl_build}" _wxl_key)
    foreach(_wxl_name IN ITEMS archives definitions options)
        string(TOUPPER "${_wxl_name}" _wxl_upper)
        set(_wxl_value "${_wxl_${_wxl_name}}")
        # MSBuild reads % $ @ ' as syntax, XML < > &; ; separates the items of
        # a list, the archives and the definitions.
        if(_wxl_value MATCHES "[%$@'<>&]")
            message(FATAL_ERROR
                "The ${_wxl_name} of ${_wxl_build} are '${_wxl_value}', with a character MSBuild "
                "or XML reads as syntax (% $ @ ' < > &).")
        endif()
        if(_wxl_name STREQUAL "options")
            string(REPLACE ";" " " _wxl_value "${_wxl_value}")
        endif()
        set(WXL_MSBUILD_${_wxl_key}_${_wxl_upper} "${_wxl_value}")
    endforeach()
endforeach()

set(_wxl_prefix "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}")
foreach(_wxl_file IN ITEMS WinXL.props WinXL.targets)
    message(STATUS "Installing: ${_wxl_prefix}/${_wxl_file}")
    configure_file("${_WXL_MSBUILD_TEMPLATES}/${_wxl_file}.in" "${_wxl_prefix}/${_wxl_file}" @ONLY)
endforeach()
