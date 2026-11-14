# /cmake/winxl-provider.cmake
#
# The provider of the NuGet package WinXL for find_package(wxl), copied into
# the repository of an application. The application includes it through its
# preset, before its first project():
#
#     "cacheVariables": {
#         "CMAKE_PROJECT_TOP_LEVEL_INCLUDES": "${sourceDir}/cmake/winxl-provider.cmake"
#     }
#
# and its CMakeLists.txt says only which wxl it needs:
#
#     find_package(wxl 0.1.0 CONFIG REQUIRED)
#
# The file registers a dependency provider of CMake for find_package. On
# find_package(wxl <version> ...) it restores the package WinXL with NuGet and
# hands find_package the CMake configuration of the restored package; every
# other find_package passes through to CMake untouched. The file does not
# depend on the version of wxl: the helpers, the profiles and the generator
# come with the package.
#
# The restore. A helper project, <build>/_winxl/restore.vcxproj, references
# WinXL at the version find_package names, as the least version it accepts,
# and MSBuild restores it:
#
#     MSBuild.exe restore.vcxproj -t:Restore
#
# MSBuild and not dotnet: a C++ project restores as the native target
# framework, the one the package is made for, and its targets come with the
# C++ build tools, which an application on wxl has installed in any case --
# the configuration of the package restores the packages of the Windows App
# SDK with the same MSBuild. dotnet restore does not evaluate a C++ project.
# MSBuild is the one of the Visual Studio developer environment the configure
# runs in (VSINSTALLDIR), which the compiler needs as well.
#
# What the restore reads is the application's: nuget.config at the root of the
# application -- the sources, the mapping of packages to sources, the global
# packages folder -- when there is one, otherwise the configuration NuGet finds
# itself; and the lock file packages.lock.json at the root, which records the
# version the restore took and every package WinXL depends on, and which the
# application commits. With RestoreLockedMode=true in the environment, as on
# CI, the restore takes the versions of the lock file and stops when the
# reference does not match it; a locked configure without the lock file stops
# here, since NuGet would write one instead of checking it.
#
# The version is the one find_package names, as it is: NuGet takes the lowest
# version of the sources that is not below it, and the lock file keeps that
# version until the minimum changes or the lock file is removed.
#
# The directory of the package is the one obj/project.assets.json of the
# restore names, and find_package then reads its build/native as the prefix of
# the package:
#
#     find_package(wxl <arguments> BYPASS_PROVIDER PATHS <package>/build/native NO_DEFAULT_PATH)
#
# A build without the provider -- wxl from its sources, add_subdirectory or
# FetchContent -- is another preset of the application, without this file.

include_guard(GLOBAL)

if(CMAKE_VERSION VERSION_LESS 3.24)
    message(FATAL_ERROR
        "winxl-provider.cmake: CMake ${CMAKE_VERSION} has no dependency providers; they came "
        "with CMake 3.24.")
endif()

# The restore of WinXL at the least version `version`, the prefix of the
# restored package in `out_prefix`.
function(_winxl_restore out_prefix version)
    if("$ENV{VSINSTALLDIR}" STREQUAL "")
        message(FATAL_ERROR
            "winxl-provider.cmake: no Visual Studio developer environment (VSINSTALLDIR is not "
            "set). The package WinXL is restored by the MSBuild of that environment: configure "
            "from a Developer Command Prompt or Developer PowerShell.")
    endif()
    file(TO_CMAKE_PATH "$ENV{VSINSTALLDIR}" installation)
    string(REGEX REPLACE "/+$" "" installation "${installation}")
    set(msbuild "${installation}/MSBuild/Current/Bin/amd64/MSBuild.exe")
    if(NOT EXISTS "${msbuild}")
        message(FATAL_ERROR
            "winxl-provider.cmake: no MSBuild at ${msbuild}, in the Visual Studio installation of "
            "the developer environment. Install the C++ build tools into it.")
    endif()

    set(helper "${CMAKE_BINARY_DIR}/_winxl")
    set(lock "${CMAKE_SOURCE_DIR}/packages.lock.json")
    set(config "${CMAKE_SOURCE_DIR}/nuget.config")
    foreach(path IN ITEMS "${helper}" "${lock}")
        # MSBuild reads these characters of a property or an item as syntax.
        if(path MATCHES "[%$@;']")
            message(FATAL_ERROR
                "winxl-provider.cmake: ${path} has a character MSBuild reads as syntax (% $ @ ; '); "
                "move the application or its build directory.")
        endif()
    endforeach()

    string(TOLOWER "$ENV{RestoreLockedMode}" locked)
    if(locked STREQUAL "true" AND NOT EXISTS "${lock}")
        message(FATAL_ERROR
            "winxl-provider.cmake: RestoreLockedMode is true in the environment, and there is no "
            "lock file ${lock}. A locked restore without the file resolves the versions anew and "
            "writes it instead of checking them. Commit the lock file a configure without "
            "RestoreLockedMode writes.")
    endif()

    file(TO_NATIVE_PATH "${lock}" lock_native)
    file(CONFIGURE OUTPUT "${helper}/restore.vcxproj" @ONLY CONTENT [=[<?xml version="1.0" encoding="utf-8"?>
<!-- Written by winxl-provider.cmake: the restore of the package WinXL, and nothing else. -->
<Project DefaultTargets="Restore" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <Configuration>Release</Configuration>
    <Platform>x64</Platform>
    <VcpkgEnabled>false</VcpkgEnabled>
    <ImportDirectoryBuildProps>false</ImportDirectoryBuildProps>
    <ImportDirectoryBuildTargets>false</ImportDirectoryBuildTargets>
    <ImportDirectoryPackagesProps>false</ImportDirectoryPackagesProps>
    <!-- The build files of the restored packages are not this project's: it is never built. -->
    <ImportProjectExtensionProps>false</ImportProjectExtensionProps>
    <ImportProjectExtensionTargets>false</ImportProjectExtensionTargets>
    <RestorePackagesWithLockFile>true</RestorePackagesWithLockFile>
    <NuGetLockFilePath>@lock_native@</NuGetLockFilePath>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Utility</ConfigurationType>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ItemGroup>
    <PackageReference Include="WinXL" Version="@version@" />
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
]=])

    # NuGet skips a restore whose inputs hash as in obj/project.nuget.cache, and
    # the lock file is not among them: a lock file a pull brought would leave the
    # result of the earlier restore. A lock file newer than that cache makes the
    # restore run for real, by the file; after every restore the cache is made
    # newer than a lock file the restore itself wrote.
    set(cache "${helper}/obj/project.nuget.cache")
    if(EXISTS "${lock}" AND EXISTS "${cache}" AND NOT "${cache}" IS_NEWER_THAN "${lock}")
        message(STATUS "WinXL: ${lock} is newer than the last restore, which runs anew.")
        file(REMOVE "${cache}")
    endif()

    set(arguments "")
    if(EXISTS "${config}")
        file(TO_NATIVE_PATH "${config}" config_native)
        list(APPEND arguments "-p:RestoreConfigFile=${config_native}")
    endif()
    file(TO_NATIVE_PATH "${helper}/restore.vcxproj" project_native)
    message(STATUS "WinXL ${version} or newer: ${helper}/restore.vcxproj")
    execute_process(
        COMMAND "${msbuild}" "${project_native}" -t:Restore
                -nologo -noAutoResponse -nodeReuse:false -verbosity:minimal ${arguments}
        WORKING_DIRECTORY "${helper}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE output)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR
            "winxl-provider.cmake: MSBuild failed (${result}) restoring WinXL ${version} or newer "
            "for ${CMAKE_SOURCE_DIR}:\n${output}")
    endif()
    file(TOUCH_NOCREATE "${cache}")

    # "libraries" names every restored package <id>/<version> with its
    # directory relative to one of the "packageFolders".
    set(assets "${helper}/obj/project.assets.json")
    file(READ "${assets}" json)
    string(JSON count LENGTH "${json}" libraries)
    set(path "")
    if(count GREATER 0)
        math(EXPR last "${count} - 1")
        foreach(i RANGE ${last})
            string(JSON key MEMBER "${json}" libraries ${i})
            string(TOLOWER "${key}" lower)
            if(lower MATCHES "^winxl/(.+)$")
                set(resolved "${CMAKE_MATCH_1}")
                string(JSON path GET "${json}" libraries "${key}" path)
            endif()
        endforeach()
    endif()
    if(path STREQUAL "")
        message(FATAL_ERROR "winxl-provider.cmake: ${assets} names no package WinXL.")
    endif()

    set(prefix "")
    string(JSON folders LENGTH "${json}" packageFolders)
    math(EXPR last "${folders} - 1")
    foreach(i RANGE ${last})
        string(JSON folder MEMBER "${json}" packageFolders ${i})
        file(TO_CMAKE_PATH "${folder}" folder)
        string(REGEX REPLACE "/+$" "" folder "${folder}")
        if(prefix STREQUAL "" AND EXISTS "${folder}/${path}/build/native/lib/cmake/wxl/wxl-config.cmake")
            set(prefix "${folder}/${path}/build/native")
        endif()
    endforeach()
    if(prefix STREQUAL "")
        message(FATAL_ERROR
            "winxl-provider.cmake: WinXL ${resolved} is in none of the package folders of "
            "${assets} with build/native/lib/cmake/wxl/wxl-config.cmake.")
    endif()

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${lock}")
    if(EXISTS "${config}")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${config}")
    endif()
    message(STATUS "WinXL ${resolved}: ${prefix}")
    set(${out_prefix} "${prefix}" PARENT_SCOPE)
endfunction()

# A macro, so that what find_package and the configuration of the package set
# is set in the scope of the find_package(wxl) the provider answers.
macro(winxl_provide_dependency _winxl_method _winxl_name)
    if("${_winxl_name}" STREQUAL "wxl")
        set(_winxl_arguments ${ARGN})
        set(_winxl_version "")
        if(_winxl_arguments)
            list(GET _winxl_arguments 0 _winxl_version)
        endif()
        if(NOT _winxl_version MATCHES "^[0-9]+(\\.[0-9]+)*$")
            message(FATAL_ERROR
                "winxl-provider.cmake: find_package(wxl ${ARGN}) names no version. The version it "
                "names is the least version of WinXL the application accepts, and the one the "
                "restore asks for: find_package(wxl 0.1.0 CONFIG REQUIRED).")
        endif()
        _winxl_restore(_winxl_prefix "${_winxl_version}")
        # find_package takes the directory of an earlier configure, wxl_DIR in
        # the cache, before any path it is given, and that is the package of the
        # earlier restore: after a new lock file, an older version. The package
        # is the one this restore resolved.
        unset(wxl_DIR CACHE)
        find_package(wxl ${ARGN} BYPASS_PROVIDER PATHS "${_winxl_prefix}" NO_DEFAULT_PATH)
        unset(_winxl_arguments)
        unset(_winxl_version)
        unset(_winxl_prefix)
    endif()
endmacro()

cmake_language(SET_DEPENDENCY_PROVIDER winxl_provide_dependency SUPPORTED_METHODS FIND_PACKAGE)
