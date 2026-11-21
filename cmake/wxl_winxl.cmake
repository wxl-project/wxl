# /cmake/wxl_winxl.cmake
#
# The target winxl: tools/winxl, the command-line utility of the tree, a C#
# program on .NET 10 published ahead of time (Native AOT) into one
# self-contained executable, <build>/winxl/winxl.exe. Included by the root
# under WXL_WINXL.
#
# The executable is made by `dotnet publish` of the .NET SDK. Which SDK is
# fixed by tools/winxl/global.json: a released 10.0 SDK, 10.0.100 or a later
# feature band or patch of 10.0 (rollForward latestFeature), never a
# prerelease (allowPrerelease false) and never another major version, however
# new an SDK the machine also has -- without the file the publish would run
# under the newest SDK installed. The host reads global.json from its working
# directory upward, so the publish runs in tools/winxl. The configure checks
# the same choice, a released 10.0.x in `dotnet --list-sdks`, and never
# downloads or installs an SDK: without one it stops and says what to install.
#
# The tool carries its own runtime, so it does not depend on the CRT of the
# build directory; it belongs to the tool set, which a release builds in
# x64-static.
#
# The publish links the native executable with the MSVC linker of the developer
# environment the tree is built in -- ninja needs cl.exe and link.exe on PATH,
# which tools\build.ps1 imports and a developer prompt has. By default the
# ILCompiler searches for a linker of its own: its
# Microsoft.NETCore.Native.Windows.targets runs findvcvarsall.bat, which runs
# vcvarsall.bat of Visual Studio, which calls vswhere.exe by name; where the
# Visual Studio Installer directory is not on PATH, the error text of that call
# becomes the directory of the linker and the link fails. With
# IlcUseEnvironmentalTools=true the targets skip that search -- the only use of
# findvcvarsall.bat in the ILCompiler's build integration, read at the
# release/10.0 branch of dotnet/runtime -- and link with `link` from PATH, the
# libraries coming from the LIB of the same environment. Nothing falls back to
# another linker: outside the environment the Exec of "link" @link.rsp fails,
# with cmd's exit code 9009 for a command it does not find, and the publish
# stops with MSBuild's MSB3073.
#
# Always Release, whatever the configuration of the build: the publish is the
# same for each, and one executable serves the build directory. Its bin/ and
# obj/ go under <build>/winxl/artifacts (--artifacts-path), not into the
# sources. The version `winxl --version` prints is that of project() of the
# root, passed to the publish as the MSBuild property WxlVersion.

include_guard(GLOBAL)

find_program(WXL_DOTNET dotnet DOC "The .NET host whose SDK publishes winxl")
if(NOT WXL_DOTNET)
    message(FATAL_ERROR
        "WXL_WINXL is ON and no dotnet is found on PATH: winxl is built by the .NET SDK 10. "
        "Install the .NET SDK 10 (https://dotnet.microsoft.com/download/dotnet/10.0), or set "
        "WXL_DOTNET to its dotnet.exe, or turn WXL_WINXL off.")
endif()

execute_process(
    COMMAND "${WXL_DOTNET}" --list-sdks
    OUTPUT_VARIABLE _wxl_dotnet_sdks
    ERROR_VARIABLE _wxl_dotnet_error
    RESULT_VARIABLE _wxl_dotnet_result)
if(NOT _wxl_dotnet_result EQUAL 0)
    message(FATAL_ERROR
        "${WXL_DOTNET} --list-sdks failed (${_wxl_dotnet_result}): ${_wxl_dotnet_error}")
endif()
# One line per SDK: "<version> [<directory>]". A released 10.0 SDK is a version
# 10.0.<n> followed by the directory; a prerelease has a label after a -
# (10.0.100-rc.1.25451.107), which global.json does not admit.
if(NOT _wxl_dotnet_sdks MATCHES "(^|\n)10\\.0\\.[0-9]+ \\[")
    string(STRIP "${_wxl_dotnet_sdks}" _wxl_dotnet_sdks)
    string(REPLACE "\n" "; " _wxl_dotnet_sdks "${_wxl_dotnet_sdks}")
    if(_wxl_dotnet_sdks STREQUAL "")
        set(_wxl_dotnet_sdks "none")
    endif()
    message(FATAL_ERROR
        "WXL_WINXL is ON and ${WXL_DOTNET} has no released .NET SDK 10.0, which builds winxl "
        "(net10.0, Native AOT; tools/winxl/global.json). Its SDKs: ${_wxl_dotnet_sdks}. A "
        "prerelease of 10.0 does not count, nor does an SDK of another major version. Install "
        "the .NET SDK 10.0 (https://dotnet.microsoft.com/download/dotnet/10.0), or turn "
        "WXL_WINXL off.")
endif()

set(_wxl_winxl_source "${CMAKE_CURRENT_LIST_DIR}/../tools/winxl")
cmake_path(NORMAL_PATH _wxl_winxl_source)
set(_wxl_winxl_dir "${CMAKE_CURRENT_BINARY_DIR}/winxl")
set(_wxl_winxl_exe "${_wxl_winxl_dir}/winxl.exe")

# A reproducible build (WXL_REPRODUCIBLE_BUILD, the root CMakeLists.txt) is a
# ContinuousIntegrationBuild of the SDK: the compiler maps the paths of the
# sources and, by winxl.csproj, of the artifacts directory, and the linker gets
# /Brepro and names the .pdb without its directory.
set(_wxl_winxl_reproducible "")
if(WXL_REPRODUCIBLE_BUILD)
    set(_wxl_winxl_reproducible -p:ContinuousIntegrationBuild=true)
endif()

file(GLOB _wxl_winxl_sources CONFIGURE_DEPENDS
    "${_wxl_winxl_source}/*.cs" "${_wxl_winxl_source}/*.csproj"
    "${_wxl_winxl_source}/global.json" "${_wxl_winxl_source}/Templates/*")

# The macros winxl refuses as names of libraries (tools/winxl/Names.cs): those
# defined at the end of a source of a solution on wxl that includes the system
# headers of cmake/header-macros.cpp and then the public headers of wxl, with
# what such a source gets -- the language of the tree (cmake/wxl_language.cmake),
# the definitions and the options of wxl.core in each configuration, which
# cmake/wxl_msbuild.cmake writes into WinXL.targets, and the definitions of the
# character set of the projects winxl writes -- for x64, the one platform of the
# package. cmake/Write-HeaderMacros.ps1 preprocesses it with the compiler of the
# build, whose developer environment names the headers of the Windows SDK and of
# the toolset (INCLUDE). The headers it read are the dependencies of the step
# (DEPFILE), with the probe, the script and the settings, so the names are
# collected again only when one of them changes.
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR NOT CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64")
    message(FATAL_ERROR
        "WXL_WINXL is ON and the compiler is ${CMAKE_CXX_COMPILER_ID} for "
        "'${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}': winxl embeds the macros a source on wxl defines, "
        "which MSVC for x64 preprocesses, the compiler and the platform of the package WinXL.")
endif()
if("$ENV{INCLUDE}" STREQUAL "")
    message(FATAL_ERROR
        "WXL_WINXL is ON and the environment has no INCLUDE: winxl embeds the macros a source on "
        "wxl defines, which the compiler preprocesses with the headers of the Windows SDK and of "
        "the MSVC toolset the developer environment names. Configure from a Developer Command "
        "Prompt or through tools\\build.ps1.")
endif()

# The projects winxl writes take the definitions of their character set from
# MSBuild: Unicode gives UNICODE and _UNICODE.
file(STRINGS "${_wxl_winxl_source}/Templates/project.vcxproj" _wxl_winxl_charset REGEX "<CharacterSet>")
if(NOT _wxl_winxl_charset MATCHES "^[ \t]*<CharacterSet>Unicode</CharacterSet>[ \t]*$")
    message(FATAL_ERROR
        "tools/winxl/Templates/project.vcxproj sets the character set '${_wxl_winxl_charset}', and "
        "cmake/wxl_winxl.cmake gives the probe of the macros the definitions of one "
        "<CharacterSet>Unicode</CharacterSet>, UNICODE and _UNICODE.")
endif()

# The public headers of wxl a source includes: the include directories of the
# header sets of wxl.core and of wxl.ui.common, which the package installs into
# include\wxl and include\wxl\ui, and the headers at the top of the set of
# wxl.ui.common but the *.impl.h, which only the sources of a profile compile,
# with the projection. What they include of impl/ and of wxl.core comes with
# them.
set(_wxl_winxl_settings "option ${CMAKE_CXX${WXL_CXX_STANDARD}_STANDARD_COMPILE_OPTION}\n")
foreach(_wxl_winxl_option IN LISTS WXL_LANGUAGE_OPTIONS)
    string(APPEND _wxl_winxl_settings "option ${_wxl_winxl_option}\n")
endforeach()
string(APPEND _wxl_winxl_settings "define UNICODE\ndefine _UNICODE\n")
set(_wxl_winxl_nl "\n")
foreach(_wxl_winxl_property IN ITEMS INTERFACE_COMPILE_OPTIONS INTERFACE_COMPILE_DEFINITIONS)
    if(_wxl_winxl_property STREQUAL "INTERFACE_COMPILE_OPTIONS")
        set(_wxl_winxl_key option)
    else()
        set(_wxl_winxl_key define)
    endif()
    string(APPEND _wxl_winxl_settings
        "$<JOIN:$<LIST:TRANSFORM,$<TARGET_PROPERTY:wxl.core,${_wxl_winxl_property}>,PREPEND,${_wxl_winxl_key} >,${_wxl_winxl_nl}>\n")
endforeach()
foreach(_wxl_winxl_target IN ITEMS wxl.core wxl.ui.common)
    if(NOT TARGET ${_wxl_winxl_target})
        continue()
    endif()
    get_target_property(_wxl_winxl_dirs ${_wxl_winxl_target} HEADER_DIRS_headers)
    if(NOT _wxl_winxl_dirs)
        message(FATAL_ERROR
            "${_wxl_winxl_target} has no header set 'headers', whose directories the probe of the "
            "macros of winxl includes from, as the package does.")
    endif()
    foreach(_wxl_winxl_header_dir IN LISTS _wxl_winxl_dirs)
        string(APPEND _wxl_winxl_settings "include ${_wxl_winxl_header_dir}\n")
    endforeach()
endforeach()
if(TARGET wxl.ui.common)
    get_target_property(_wxl_winxl_ui_dirs wxl.ui.common HEADER_DIRS_headers)
    get_target_property(_wxl_winxl_ui_headers wxl.ui.common HEADER_SET_headers)
    foreach(_wxl_winxl_header IN LISTS _wxl_winxl_ui_headers)
        cmake_path(GET _wxl_winxl_header PARENT_PATH _wxl_winxl_parent)
        if(_wxl_winxl_parent IN_LIST _wxl_winxl_ui_dirs AND NOT _wxl_winxl_header MATCHES "\\.impl\\.h$")
            string(APPEND _wxl_winxl_settings "header ${_wxl_winxl_header}\n")
        endif()
    endforeach()
endif()

# One file of settings for each configuration, whose definitions and options
# are those of wxl.core in it.
get_property(_wxl_winxl_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_wxl_winxl_multi)
    set(_wxl_winxl_configs ${CMAKE_CONFIGURATION_TYPES})
else()
    set(_wxl_winxl_configs "${CMAKE_BUILD_TYPE}")
endif()
file(GENERATE OUTPUT "${_wxl_winxl_dir}/header-macros-$<CONFIG>.settings" CONTENT "${_wxl_winxl_settings}")
set(_wxl_winxl_settings_files "")
foreach(_wxl_winxl_config IN LISTS _wxl_winxl_configs)
    list(APPEND _wxl_winxl_settings_files "${_wxl_winxl_dir}/header-macros-${_wxl_winxl_config}.settings")
endforeach()
list(JOIN _wxl_winxl_settings_files "|" _wxl_winxl_settings_argument)

# Windows PowerShell 5.1, which every Windows has and the script is written for.
find_program(POWERSHELL_EXE NAMES powershell)
if(NOT POWERSHELL_EXE)
    message(FATAL_ERROR
        "WXL_WINXL is ON and no PowerShell is found: the build of winxl collects the macros of "
        "the headers with it. Install PowerShell (powershell.exe, Windows PowerShell 5.1, on PATH).")
endif()
set(_wxl_winxl_macros "${_wxl_winxl_dir}/header-macros.txt")
add_custom_command(
    OUTPUT "${_wxl_winxl_macros}"
    COMMAND "${POWERSHELL_EXE}" -NoProfile -ExecutionPolicy Bypass -File
            "${CMAKE_CURRENT_LIST_DIR}/Write-HeaderMacros.ps1"
            -Compiler "${CMAKE_CXX_COMPILER}" -Probe "${CMAKE_CURRENT_LIST_DIR}/header-macros.cpp"
            -Settings "${_wxl_winxl_settings_argument}" -Out "${_wxl_winxl_macros}"
            -Depfile "${_wxl_winxl_dir}/header-macros.d"
    DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Write-HeaderMacros.ps1" "${CMAKE_CURRENT_LIST_DIR}/header-macros.cpp"
            ${_wxl_winxl_settings_files}
    DEPFILE "${_wxl_winxl_dir}/header-macros.d"
    COMMENT "winxl: the macros a source on wxl defines, preprocessed by ${CMAKE_CXX_COMPILER}"
    VERBATIM)

add_custom_command(
    OUTPUT "${_wxl_winxl_exe}"
    COMMAND "${WXL_DOTNET}" publish "${_wxl_winxl_source}/winxl.csproj"
            -c Release -r win-x64 --nologo
            --artifacts-path "${_wxl_winxl_dir}/artifacts"
            -o "${_wxl_winxl_dir}"
            "-p:WxlVersion=${WXL_VERSION}"
            "-p:WxlHeaderMacros=${_wxl_winxl_macros}"
            -p:IlcUseEnvironmentalTools=true
            ${_wxl_winxl_reproducible}
    DEPENDS ${_wxl_winxl_sources} "${_wxl_winxl_macros}"
    WORKING_DIRECTORY "${_wxl_winxl_source}"
    COMMENT "dotnet publish: winxl ${WXL_VERSION} -> ${_wxl_winxl_exe}"
    VERBATIM)

add_custom_target(winxl ALL
    DEPENDS "${_wxl_winxl_exe}"
    SOURCES ${_wxl_winxl_sources})
