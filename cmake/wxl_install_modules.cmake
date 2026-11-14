# /cmake/wxl_install_modules.cmake
#
# A step of the install of wxl (cmake/wxl_install.cmake), run after the export
# is installed: the module interface units the export names by the files of the
# build are named by their installed copies instead. Given by the step before
# it:
#
#   _WXL_INSTALL_MODULES_DIR   the cxx-modules directory of the export, under
#                              the prefix;
#   _WXL_INSTALL_MODULE_DIRS   <base directory>|<destination> for every base
#                              directory of a module file set, the destination
#                              relative to the prefix.
#
# Every entry <module>=<base directory>/<file> of IMPORTED_CXX_MODULES_<CONFIG>
# becomes <module>=${_IMPORT_PREFIX}/<destination>/<file>. An export that names
# the installed files already -- of a CMake that does so itself -- is left as it
# is. An entry that names no file under the prefix afterwards stops the install.

set(_wxl_dir "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_WXL_INSTALL_MODULES_DIR}")
file(GLOB _wxl_files "${_wxl_dir}/target-*.cmake")
foreach(_wxl_file IN LISTS _wxl_files)
    file(READ "${_wxl_file}" _wxl_text)
    set(_wxl_new "${_wxl_text}")
    foreach(_wxl_pair IN LISTS _WXL_INSTALL_MODULE_DIRS)
        string(FIND "${_wxl_pair}" "|" _wxl_bar)
        string(SUBSTRING "${_wxl_pair}" 0 ${_wxl_bar} _wxl_base)
        math(EXPR _wxl_bar "${_wxl_bar} + 1")
        string(SUBSTRING "${_wxl_pair}" ${_wxl_bar} -1 _wxl_destination)
        string(REPLACE "=${_wxl_base}/" "=\${_IMPORT_PREFIX}/${_wxl_destination}/"
            _wxl_new "${_wxl_new}")
    endforeach()

    string(REGEX MATCHALL "\"[^\"=]+=[^\"]*\"" _wxl_entries "${_wxl_new}")
    foreach(_wxl_entry IN LISTS _wxl_entries)
        string(FIND "${_wxl_entry}" "=\${_IMPORT_PREFIX}/" _wxl_at)
        if(_wxl_at EQUAL -1)
            message(FATAL_ERROR
                "${_wxl_file}: the module ${_wxl_entry} is not under the prefix. The base "
                "directories of the module file sets are ${_WXL_INSTALL_MODULE_DIRS} "
                "(cmake/wxl_install.cmake).")
        endif()
    endforeach()

    if(NOT _wxl_new STREQUAL _wxl_text)
        file(WRITE "${_wxl_file}" "${_wxl_new}")
    endif()
endforeach()
