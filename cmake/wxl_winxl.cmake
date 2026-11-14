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

file(GLOB _wxl_winxl_sources CONFIGURE_DEPENDS
    "${_wxl_winxl_source}/*.cs" "${_wxl_winxl_source}/*.csproj"
    "${_wxl_winxl_source}/global.json")

add_custom_command(
    OUTPUT "${_wxl_winxl_exe}"
    COMMAND "${WXL_DOTNET}" publish "${_wxl_winxl_source}/winxl.csproj"
            -c Release -r win-x64 --nologo
            --artifacts-path "${_wxl_winxl_dir}/artifacts"
            -o "${_wxl_winxl_dir}"
            "-p:WxlVersion=${WXL_VERSION}"
            -p:IlcUseEnvironmentalTools=true
    DEPENDS ${_wxl_winxl_sources}
    WORKING_DIRECTORY "${_wxl_winxl_source}"
    COMMENT "dotnet publish: winxl ${WXL_VERSION} -> ${_wxl_winxl_exe}"
    VERBATIM)

add_custom_target(winxl ALL
    DEPENDS "${_wxl_winxl_exe}"
    SOURCES ${_wxl_winxl_sources})
