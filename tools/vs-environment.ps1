# The developer environment of MSVC for a script run from a plain shell,
# dot-sourced by tools\build.ps1 and tools\package\build.ps1.

# Runs <command> in a child cmd, reads back the variables it set, and applies
# them to the current process.
function Import-CmdEnvironment([string]$Command) {
    & "$env:ComSpec" /c "$Command >nul 2>nul && set" | ForEach-Object {
        if ($_ -match '^([^=]+)=(.*)$') {
            Set-Item -Path "env:$($Matches[1])" -Value $Matches[2]
        }
    }
}

# vcvars64.bat of the newest Visual Studio with the C++ toolset and the C++
# WinUI tools, asked for the MSVC toolset <Toolset> (-vcvars_ver).
function Import-VisualStudioEnvironment([string]$Toolset) {
    if ($env:VSCMD_VER) { return }  # already inside a developer prompt

    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) { throw "vswhere.exe not found: $vswhere" }

    # -prerelease so an Insiders/preview installation counts; the tree needs a
    # toolset new enough for `import std;`, which is where those tend to be.
    # The installation also needs the C++ WinUI tools, which the configure
    # checks (cmake/wxl_packages.cmake): the component of Visual Studio or the
    # component group of Build Tools. vswhere applies -requiresAny to every ID
    # it is given, so the two conditions are two lists, and the newest
    # installation of the first list that is also in the second is taken.
    $withCpp = @(& $vswhere -prerelease -products * -sort `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath)
    $withWinUI = @(& $vswhere -prerelease -products * -requiresAny `
        -requires Microsoft.VisualStudio.Component.WindowsAppSdkSupport.Cpp `
                  Microsoft.VisualStudio.ComponentGroup.WindowsAppDevelopment.VC.BuildTools `
        -property installationPath)
    $install = $withCpp | Where-Object { $withWinUI -contains $_ } | Select-Object -First 1
    if (-not $install) {
        throw ('No Visual Studio installation with the C++ toolset and the C++ WinUI tools was found. ' +
            'Visual Studio 2026: the workload "WinUI application development" with "C++ WinUI app ' +
            'development tools" (Microsoft.VisualStudio.Component.WindowsAppSdkSupport.Cpp). Build Tools ' +
            'for Visual Studio 2026: the workload "WinUI application development build tools" with ' +
            '"C++ WinUI app development build tools" ' +
            '(Microsoft.VisualStudio.ComponentGroup.WindowsAppDevelopment.VC.BuildTools).')
    }

    $vcvars = Join-Path $install 'VC\Auxiliary\Build\vcvars64.bat'
    if (-not (Test-Path $vcvars)) { throw "vcvars64.bat not found: $vcvars" }

    Import-CmdEnvironment "`"$vcvars`" -vcvars_ver=$Toolset"
}
