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

# The macros winxl refuses as names of libraries (tools/winxl/Names.cs): the
# names of the #defines of the headers a source of an application includes,
# those of the Windows SDK -- ucrt, um, shared, winrt -- and of the MSVC toolset
# of the developer environment the tree is built in, which also compiles the
# tree: WindowsSdkDir and WindowsSDKVersion name the SDK, as for the restore of
# cmake/wxl_packages.cmake, and VCToolsInstallDir the toolset. A header is a
# .h, .hpp, .hxx or .inl file, or a file without an extension (the standard
# library). The list of the headers is rewritten only when it changes, and the
# names are collected again only when it or a header changes: the headers are
# the dependencies of the step, a few thousand files that ninja only stats.
foreach(_wxl_winxl_variable WindowsSdkDir WindowsSDKVersion VCToolsInstallDir)
    if("$ENV{${_wxl_winxl_variable}}" STREQUAL "")
        message(FATAL_ERROR
            "WXL_WINXL is ON and the environment has no ${_wxl_winxl_variable}: winxl embeds the "
            "macros of the headers of the Windows SDK and of the MSVC toolset of the developer "
            "environment. Configure from a Developer Command Prompt or through tools\\build.ps1.")
    endif()
endforeach()
file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}" _wxl_winxl_kits)
string(REGEX REPLACE "[\\/]+$" "" _wxl_winxl_sdk "$ENV{WindowsSDKVersion}")
file(TO_CMAKE_PATH "$ENV{VCToolsInstallDir}" _wxl_winxl_toolset)
set(_wxl_winxl_header_dirs
    "${_wxl_winxl_kits}/Include/${_wxl_winxl_sdk}/ucrt"
    "${_wxl_winxl_kits}/Include/${_wxl_winxl_sdk}/um"
    "${_wxl_winxl_kits}/Include/${_wxl_winxl_sdk}/shared"
    "${_wxl_winxl_kits}/Include/${_wxl_winxl_sdk}/winrt"
    "${_wxl_winxl_toolset}/include")
set(_wxl_winxl_headers "")
foreach(_wxl_winxl_header_dir IN LISTS _wxl_winxl_header_dirs)
    cmake_path(NORMAL_PATH _wxl_winxl_header_dir)
    if(NOT IS_DIRECTORY "${_wxl_winxl_header_dir}")
        message(FATAL_ERROR
            "No ${_wxl_winxl_header_dir}: winxl embeds the macros of the headers there, of the "
            "Windows SDK and the MSVC toolset the developer environment names (WindowsSdkDir, "
            "WindowsSDKVersion, VCToolsInstallDir).")
    endif()
    file(GLOB_RECURSE _wxl_winxl_files LIST_DIRECTORIES false "${_wxl_winxl_header_dir}/*")
    list(FILTER _wxl_winxl_files INCLUDE REGEX "(/[^./]+|\\.([hH]|[hH][pP][pP]|[hH][xX][xX]|[iI][nN][lL]))$")
    list(APPEND _wxl_winxl_headers ${_wxl_winxl_files})
endforeach()
list(SORT _wxl_winxl_headers)
set(_wxl_winxl_header_list "${_wxl_winxl_dir}/header-macros.headers")
# The paths go in as the value of one variable, which @ONLY substitutes once.
list(JOIN _wxl_winxl_headers "\n" _wxl_winxl_text)
file(CONFIGURE OUTPUT "${_wxl_winxl_header_list}" CONTENT "@_wxl_winxl_text@\n" @ONLY)

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
            -Headers "${_wxl_winxl_header_list}" -Out "${_wxl_winxl_macros}"
    DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Write-HeaderMacros.ps1" "${_wxl_winxl_header_list}"
            ${_wxl_winxl_headers}
    COMMENT "winxl: the macros of the headers of Windows SDK ${_wxl_winxl_sdk} and ${_wxl_winxl_toolset}"
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
