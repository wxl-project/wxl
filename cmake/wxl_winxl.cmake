# /cmake/wxl_winxl.cmake
#
# The target winxl: tools/winxl, the command-line utility of the tree, a C#
# program on .NET 10 published ahead of time (Native AOT) into one
# self-contained executable, <build>/winxl/winxl.exe. Included by the root
# under WXL_WINXL.
#
# The executable is made by `dotnet publish` of the .NET SDK. The configure
# finds the SDK and checks its version, and never downloads or installs one:
# without a `dotnet` that lists an SDK of major version 10 it stops and says
# what to install. The tool carries its own runtime, so it does not depend on
# the CRT of the build directory; it belongs to the tool set, which a release
# builds in x64-static. The publish links the native executable with the MSVC
# linker of a Visual Studio installation with the C++ toolset, which the tree
# needs anyway.
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
# One line per SDK: "<version> [<directory>]".
if(NOT _wxl_dotnet_sdks MATCHES "(^|\n)10\\.[0-9]+\\.[0-9]+")
    string(STRIP "${_wxl_dotnet_sdks}" _wxl_dotnet_sdks)
    string(REPLACE "\n" "; " _wxl_dotnet_sdks "${_wxl_dotnet_sdks}")
    if(_wxl_dotnet_sdks STREQUAL "")
        set(_wxl_dotnet_sdks "none")
    endif()
    message(FATAL_ERROR
        "WXL_WINXL is ON and ${WXL_DOTNET} has no .NET SDK of major version 10, which builds "
        "winxl (net10.0, Native AOT). Its SDKs: ${_wxl_dotnet_sdks}. Install the .NET SDK 10 "
        "(https://dotnet.microsoft.com/download/dotnet/10.0), or turn WXL_WINXL off.")
endif()

set(_wxl_winxl_source "${CMAKE_CURRENT_LIST_DIR}/../tools/winxl")
cmake_path(NORMAL_PATH _wxl_winxl_source)
set(_wxl_winxl_dir "${CMAKE_CURRENT_BINARY_DIR}/winxl")
set(_wxl_winxl_exe "${_wxl_winxl_dir}/winxl.exe")

file(GLOB _wxl_winxl_sources CONFIGURE_DEPENDS
    "${_wxl_winxl_source}/*.cs" "${_wxl_winxl_source}/*.csproj")

add_custom_command(
    OUTPUT "${_wxl_winxl_exe}"
    COMMAND "${WXL_DOTNET}" publish "${_wxl_winxl_source}/winxl.csproj"
            -c Release -r win-x64 --nologo
            --artifacts-path "${_wxl_winxl_dir}/artifacts"
            -o "${_wxl_winxl_dir}"
            "-p:WxlVersion=${wxl-project_VERSION}"
    DEPENDS ${_wxl_winxl_sources}
    COMMENT "dotnet publish: winxl ${wxl-project_VERSION} -> ${_wxl_winxl_exe}"
    VERBATIM)

add_custom_target(winxl ALL
    DEPENDS "${_wxl_winxl_exe}"
    SOURCES ${_wxl_winxl_sources})
