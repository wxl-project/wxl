# Builds the NuGet package WinXL from this tree with one command, the way
# .github/workflows/package.yml builds it:
#
#     tools\package\build.ps1 [-Version <version>] [-Out <directory>]
#         [-Work <directory>] [-Jobs <n>] [-Define <name>=<value>,...]
#         [-Toolset <MSVC toolset>]
#
# The parts are the jobs of package.yml, in turn:
#
#   libs   x64-dynamic (the CRT in a DLL) and x64-static (the static CRT),
#          each configured, built Debug and Release and installed, with the
#          debugging information in the objects of the archives (/Z7);
#   tools  the generator winui-srcgen, the profile editor wxl.gen.ui and the
#          utility winxl, x64-static Release, run and tested;
#   pack   the two installs merged into one prefix with the tool set in bin,
#          packed by pack.cmake beside this script into WinXL.<version>.nupkg
#          and checked to be reproducible.
#
# Each part builds in a directory of its own under <Work> -- x64-dynamic,
# x64-static and tools -- and not in build\<preset>, the build directory of
# a working tree, whose cache the options of the package would replace. The
# installs, the tool set and the merged prefix are <Work>\install-md,
# install-mt, tools-x64 and stage, removed and made anew on every run; the
# build directories are kept, so a second run builds only what changed. <Work>
# is build\package at the root of the tree unless -Work names it; the package
# is written into <Out>, <Work>\package unless -Out names it.
#
# -Step runs one part the way a job of CI runs it, at the root of the tree and
# in the layout of the workflows -- the actions .github/actions/package-libs,
# package-tools and package-pack call it so:
#
#   -Step Libs -Preset <preset>  build\<preset>, installed into stage
#   -Step Tools                  build\x64-static, collected into
#                                artifact\wxl-tools-x64
#   -Step Pack                   install-md, install-mt (stage of Libs with
#                                x64-dynamic and x64-static) and tools-x64 (the
#                                tool set of Tools) merged into stage and
#                                packed into package
#
# The version is the version of the commit (cmake/wxl_package_version.cmake)
# unless -Version names it, and the commit the manifest names is HEAD unless
# -Commit names it: uncommitted changes are built into the package and change
# neither.
#
# The developer environment of MSVC is the one the shell has (VSCMD_VER), the
# one DEVCMD with the arguments VCVARS gives when they are set -- as the
# workflows of the tree set them --, or the one of vcvars64.bat, which
# tools\vs-environment.ps1 finds, with the toolset -Toolset. The restore of
# each configure runs in locked mode (RestoreLockedMode), as in the workflows.
# The package step needs no developer environment: CMake and the .NET SDK of
# global.json beside this script on PATH.

[CmdletBinding()]
param(
    [ValidateSet('All', 'Libs', 'Tools', 'Pack')]
    [string]$Step = 'All',

    # The configure preset of -Step Libs: x64-dynamic or x64-static.
    [ValidateSet('x64-dynamic', 'x64-static')]
    [string]$Preset,

    [string]$Version,
    [string]$Commit,
    [string]$Work,
    [string]$Out,

    # Parallel jobs of each build; 0 leaves the number to Ninja.
    [int]$Jobs = 0,

    # The MSVC toolset vcvars64.bat is asked for, as in tools\build.ps1.
    [string]$Toolset = '14.53',

    # Cache entries given to every configure before the options of the
    # package, without the -D: WXL_LOCAL_SOURCE_DIR=<directory>, say.
    [string[]]$Define
)

$ErrorActionPreference = 'Stop'
# Without .. in it: the check of the package searches the files for this path.
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..')).TrimEnd('\')
. (Join-Path $root 'tools\vs-environment.ps1')

if (($Step -eq 'Libs') -ne [bool]$Preset) { throw '-Preset names the build of -Step Libs, and only of it.' }
if ($Step -ne 'All' -and ($Work -or $Out)) {
    throw "-Work and -Out are for the whole package: -Step $Step works where the job of CI does."
}

$defines = @(foreach ($d in $Define) { "-D$d" })
$parallel = @(if ($Jobs -gt 0) { '-j', $Jobs })

# A part of the log: a group the log of a workflow folds, a heading on a
# machine.
function Invoke-Part([string]$Name, [scriptblock]$Body) {
    if ($env:GITHUB_ACTIONS -eq 'true') { Write-Host "::group::$Name" }
    else { Write-Host "== $Name ($(Get-Date -Format 'HH:mm:ss'))" -ForegroundColor Cyan }
    try { & $Body }
    finally { if ($env:GITHUB_ACTIONS -eq 'true') { Write-Host '::endgroup::' } }
}

function Assert-ExitCode([string]$What) {
    if ($LASTEXITCODE -ne 0) { throw "$What exited with $LASTEXITCODE" }
}

function Import-DeveloperEnvironment {
    if ($env:VSCMD_VER) { return }
    if ($env:DEVCMD) { Import-CmdEnvironment "`"$env:DEVCMD`" -arch=x64 -host_arch=x64 $env:VCVARS" }
    else { Import-VisualStudioEnvironment $Toolset }
}

# The libraries of one build directory, installed into <Prefix>.
function Build-Libs([string]$Preset, [string]$BuildDir, [string]$Prefix) {
    $targets = 'wxl.core', 'fmt.module', 'wxl.highlight', 'wxl.bb', 'wxl.rsdn', 'wxl.ui.common', 'winui-srcgen'

    # The library and its install rules, and nothing an install does not
    # take: no samples, no sandbox, no site, no tests. Debug and Release are
    # built and installed with the debugging information in the objects of the
    # archives (/Z7, CMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded) -- a package
    # has no place for the .pdb files of a compiler. Reproducible
    # (WXL_REPRODUCIBLE_BUILD): the archives are the same bytes from the same
    # commit and toolset, wherever the tree is checked out.
    Invoke-Part "Configure $Preset" {
        cmake --preset $Preset -S $root -B $BuildDir @defines -DWXL_INSTALL=ON "-DWXL_PACKAGE_VERSION=$Version" -DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded -DWXL_REPRODUCIBLE_BUILD=ON -DWXL_SAMPLES=OFF -DWXL_SANDBOX=OFF -DWXL_BUILD_DOCS=OFF
        Assert-ExitCode 'the configure'
    }

    # Every compile of both configurations takes /Z7 and none /Zi, which would
    # leave the debugging information in a .pdb beside the build.
    Invoke-Part "Check the debugging information of $Preset" {
        foreach ($config in 'Debug', 'Release') {
            $rules = Join-Path $BuildDir "CMakeFiles\impl-$config.ninja"
            if (Select-String -Path $rules -Pattern '[-/]Zi\b' -Quiet) { throw "$rules compiles with /Zi" }
            if (-not (Select-String -Path $rules -Pattern '[-/]Z7\b' -Quiet)) { throw "$rules compiles nothing with /Z7" }
        }
    }

    foreach ($config in 'Debug', 'Release') {
        Invoke-Part "Build $Preset $config" {
            cmake --build $BuildDir --config $config --target @targets @parallel -- -k 0
            Assert-ExitCode "the build of $config"
        }
    }

    Invoke-Part "Install $Preset" {
        foreach ($config in 'Debug', 'Release') {
            cmake --install $BuildDir --config $config --prefix $Prefix
            Assert-ExitCode "the install of $config"
        }
    }
}

# The tool set, built in the x64-static build directory -- the CRT linked
# statically (CMakePresets.json) -- in Release, run, tested and collected into
# <ToolSet>. winxl is published by the .NET SDK 10; cmake/wxl_winxl.cmake stops
# the configure if it is missing.
function Build-Tools([string]$BuildDir, [string]$ToolSet) {
    # BUILD_TESTING for the tests of the generator and of its common library,
    # which the next steps build and run; WXL_WINXL for the utility;
    # WXL_PACKAGE_VERSION for the version winxl prints; WXL_REPRODUCIBLE_BUILD
    # for executables that are the same bytes from the same commit and toolset,
    # wherever the tree is checked out.
    Invoke-Part 'Configure the tools' {
        dotnet --list-sdks
        cmake --preset x64-static -S $root -B $BuildDir @defines -DBUILD_TESTING=ON -DWXL_WINXL=ON "-DWXL_PACKAGE_VERSION=$Version" -DWXL_REPRODUCIBLE_BUILD=ON
        Assert-ExitCode 'the configure'
    }

    Invoke-Part 'Build the tools' {
        cmake --build $BuildDir --config Release --target winui-srcgen wxl.gen.ui winxl wxl.gen.tests wxl.gen.common.tests @parallel -- -k 0
        Assert-ExitCode 'the build'
    }

    # Each executable started once: a tool linked with the static CRT has to
    # load and answer before anything relies on it. winxl answers with the
    # version it was given.
    Invoke-Part 'Run the tools' {
        & (Join-Path $BuildDir 'wxl.gen\Release\winui-srcgen.exe') --help
        Assert-ExitCode 'winui-srcgen --help'
        $printed = & (Join-Path $BuildDir 'winxl\winxl.exe') --version
        Assert-ExitCode 'winxl --version'
        if ($printed -ne $Version) { throw "winxl --version printed '$printed', not $Version" }
    }

    # The tests of the tools, each from its own directory: the preset would
    # list every program of the tree, and the ones not built would fail.
    Invoke-Part 'Test the tools' {
        foreach ($tests in 'wxl.gen\tests', 'wxl.gen.common\tests') {
            ctest --test-dir (Join-Path $BuildDir $tests) -C Release --output-on-failure --timeout 300
            Assert-ExitCode "ctest of $tests"
        }
    }

    # The executables with what they need beside them -- the editor's folder
    # holds its assets and the runtime libraries it loads -- without the
    # debugging symbols.
    Invoke-Part 'Collect the tools' {
        $set = New-Item -ItemType Directory -Force -Path $ToolSet
        Copy-Item (Join-Path $BuildDir 'wxl.gen\Release\winui-srcgen.exe') $set
        Copy-Item (Join-Path $BuildDir 'winxl\winxl.exe') $set
        $editor = New-Item -ItemType Directory -Force -Path (Join-Path $set 'wxl.gen.ui')
        Copy-Item (Join-Path $BuildDir 'wxl.gen.ui\Release\*') $editor -Recurse -Exclude '*.pdb', '*.ilk'
        Get-ChildItem $set -Recurse -File | ForEach-Object { $_.FullName.Substring($set.FullName.Length + 1) }
    }
}

# The installs <Work>\install-md and install-mt and the tool set
# <Work>\tools-x64, merged into <Work>\stage -- the archives and the export of
# each build in lib/<md|mt> and lib/cmake/wxl/<md|mt>, everything else the same
# in both, which this checks -- with the tool set in bin, and packed into
# <PackageDir>\WinXL.<version>.nupkg. <Again> takes the second pack of the
# check.
function Build-Package([string]$Work, [string]$PackageDir, [string]$Again) {
    $installMd = Join-Path $Work 'install-md'
    $installMt = Join-Path $Work 'install-mt'
    $toolSet = Join-Path $Work 'tools-x64'
    $stage = Join-Path $Work 'stage'
    $package = Join-Path $PackageDir "WinXL.$Version.nupkg"
    $commitArgs = @(if ($Commit) { "-DWXL_PACK_COMMIT=$Commit" })

    # Everything the two installs share is the same in both: the module
    # interfaces, the headers, the components, the profiles, the helpers, the
    # configuration and its version. Apart are the archives and the export
    # of each build, and the generator in bin, which the tool set replaces.
    Invoke-Part 'Merge the installs' {
        function Get-Shared($prefix) {
            $prefix = (Resolve-Path $prefix).Path
            Get-ChildItem $prefix -Recurse -File | ForEach-Object { $_.FullName.Substring($prefix.Length + 1).Replace('\', '/') } |
                Where-Object { $_ -notmatch '^(lib/(md|mt)/|lib/cmake/wxl/(md|mt)/|bin/)' } | Sort-Object
        }
        $md = @(Get-Shared $installMd)
        $mt = @(Get-Shared $installMt)
        $diff = Compare-Object $md $mt
        if ($diff) { $diff | Format-Table -AutoSize | Out-String | Write-Host; throw 'the two installs do not hold the same files' }
        foreach ($file in $md) {
            if ((Get-FileHash "$installMd/$file").Hash -ne (Get-FileHash "$installMt/$file").Hash) { throw "$file differs between the two installs" }
        }
        foreach ($variant in 'md', 'mt') {
            foreach ($dir in "lib/$variant", "lib/cmake/wxl/$variant") {
                if (-not (Test-Path (Join-Path $Work "install-$variant/$dir"))) { throw "install-$variant has no $dir" }
            }
        }
        "$($md.Count) files the same in both"

        New-Item -ItemType Directory $stage | Out-Null
        Copy-Item "$installMd/*" $stage -Recurse
        Copy-Item "$installMt/lib/mt" "$stage/lib" -Recurse
        Copy-Item "$installMt/lib/cmake/wxl/mt" "$stage/lib/cmake/wxl" -Recurse
        Remove-Item "$stage/bin" -Recurse
        New-Item -ItemType Directory "$stage/bin" | Out-Null
        Copy-Item "$toolSet/*" "$stage/bin" -Recurse
        foreach ($file in 'bin/winui-srcgen.exe', 'bin/winxl.exe') {
            if (-not (Test-Path "$stage/$file")) { throw "the tool set has no $file" }
        }
    }

    Invoke-Part 'Pack' {
        cmake "-DWXL_PACK_STAGE=$stage" "-DWXL_PACK_OUTPUT=$PackageDir" @commitArgs -P (Join-Path $PSScriptRoot 'pack.cmake')
        Assert-ExitCode 'pack.cmake'
        if (-not (Test-Path $package)) { throw "no ${package}: the version of the install is not $Version" }
    }

    # The package made again from the commit with the same toolset is the same
    # bytes but for the profile editor, whose link MSVC 14.53 does not repeat
    # (WXL_REPRODUCIBLE_BUILD, pack.cmake, docs/building.md). Without a second
    # build a run checks what it can: no archive or CMake file of lib and no
    # file of bin, the profile editor's folder among them, names the tree --
    # the builds ran at the same root, the maps of the compiler took the paths
    # out of the objects, and the editor opens the profiles and components of
    # the package, found from its own folder; every member of an archive has
    # the time -1 of lib /Brepro; and the prefix packed a second time is the
    # same bytes.
    Invoke-Part 'Check the package is reproducible' {
        $tree = $root.TrimEnd('\', '/')
        $forms = @($tree.Replace('/', '\'), $tree.Replace('\', '/'))
        $latin1 = [Text.Encoding]::Latin1
        foreach ($file in @(Get-ChildItem "$stage/lib" -Recurse -File) + @(Get-ChildItem "$stage/bin" -Recurse -File)) {
            $bytes = [IO.File]::ReadAllBytes($file.FullName)
            # One character per byte, and UTF-16 from either alignment.
            $texts = @($latin1.GetString($bytes), [Text.Encoding]::Unicode.GetString($bytes))
            if ($bytes.Length -gt 1) { $texts += [Text.Encoding]::Unicode.GetString($bytes, 1, $bytes.Length - 1) }
            foreach ($text in $texts) {
                foreach ($form in $forms) {
                    if ($text.IndexOf($form, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "$($file.FullName) names the tree $form" }
                }
            }
            if ($file.Extension -eq '.lib') {
                # !<arch>, then each member: a header of 60 bytes -- the date in
                # bytes 16 to 28, the size in 48 to 58 -- and the data, padded to
                # an even length.
                if ($latin1.GetString($bytes, 0, 8) -ne "!<arch>`n") { throw "$($file.FullName) is not an archive" }
                $at = 8
                while ($at -lt $bytes.Length) {
                    $date = $latin1.GetString($bytes, $at + 16, 12).Trim()
                    if ($date -ne '-1') { throw "$($file.FullName): a member at $at has the time $date, not -1 of lib /Brepro" }
                    $size = [long]$latin1.GetString($bytes, $at + 48, 10).Trim()
                    $at += 60 + $size + ($size % 2)
                }
            }
        }
        cmake "-DWXL_PACK_STAGE=$stage" "-DWXL_PACK_OUTPUT=$Again" @commitArgs -P (Join-Path $PSScriptRoot 'pack.cmake')
        Assert-ExitCode 'pack.cmake'
        $first = (Get-FileHash $package -Algorithm SHA512).Hash
        $second = (Get-FileHash (Join-Path $Again "WinXL.$Version.nupkg") -Algorithm SHA512).Hash
        if ($first -ne $second) { throw "the prefix packed twice gives two packages: $first and $second" }
        "SHA-512 $first"
    }
}

Push-Location $root
$lockedMode = $env:RestoreLockedMode
try {
    if (-not $Version) {
        $Version = cmake -P (Join-Path $root 'cmake\wxl_package_version.cmake')
        Assert-ExitCode 'cmake/wxl_package_version.cmake'
    }
    if ($Step -ne 'Pack') {
        Import-DeveloperEnvironment
        $env:RestoreLockedMode = 'true'
    }

    switch ($Step) {
        'Libs' { Build-Libs $Preset (Join-Path $root "build\$Preset") (Join-Path $root 'stage') }
        'Tools' { Build-Tools (Join-Path $root 'build\x64-static') (Join-Path $root 'artifact\wxl-tools-x64') }
        'Pack' {
            $temp = if ($env:RUNNER_TEMP) { $env:RUNNER_TEMP } else { [IO.Path]::GetTempPath() }
            Build-Package $root (Join-Path $root 'package') (Join-Path $temp 'WinXL-again')
        }
        'All' {
            $Work = if ($Work) { $PSCmdlet.GetUnresolvedProviderPathFromPSPath($Work) } else { Join-Path $root 'build\package' }
            $Out = if ($Out) { $PSCmdlet.GetUnresolvedProviderPathFromPSPath($Out) } else { Join-Path $Work 'package' }
            $started = Get-Date
            Write-Host "WinXL $Version from $root in $Work"
            foreach ($part in 'install-md', 'install-mt', 'tools-x64', 'stage', 'again') {
                $dir = Join-Path $Work $part
                if (Test-Path $dir) { Remove-Item $dir -Recurse -Force }
            }
            Build-Libs 'x64-dynamic' (Join-Path $Work 'x64-dynamic') (Join-Path $Work 'install-md')
            Build-Libs 'x64-static' (Join-Path $Work 'x64-static') (Join-Path $Work 'install-mt')
            Build-Tools (Join-Path $Work 'tools') (Join-Path $Work 'tools-x64')
            Build-Package $Work $Out (Join-Path $Work 'again')
            Write-Host "WinXL $Version`: $(Join-Path $Out "WinXL.$Version.nupkg") in $([int]((Get-Date) - $started).TotalMinutes) min"
        }
    }
} finally {
    $env:RestoreLockedMode = $lockedMode
    Pop-Location
}
