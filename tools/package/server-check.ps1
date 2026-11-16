# Publishes the package WinXL on a NuGet server and builds the package
# consumer against that server, as an application that takes WinXL from it:
#
#     $env:WXL_NUGET_API_KEY = '<key>'
#     tools\package\server-check.ps1 -Package <WinXL.<version>.nupkg>
#         -Source <service index> -Application <directory>
#
# The source is the service index of a NuGet v3 server: a server run on this
# machine for the check, or https://api.nuget.org/v3/index.json for a release.
# The key comes from the environment and not from the command line, which a
# shell keeps in its history. Run from a Developer PowerShell: the provider
# restores the package with the MSBuild of that environment.
#
# The steps:
#
#   1. tests/package-consumer is copied into <directory>, which is made and
#      must be new or empty, with the provider cmake/winxl-provider.cmake in its
#      cmake/; its nuget.config is written anew and names two sources, the
#      server for WinXL and nuget.org for everything else (only nuget.org when
#      the server is nuget.org).
#   2. dotnet nuget push puts the package on the server, through the source of
#      that nuget.config. -NoPush skips it, for a package that is on the server
#      already: a version is pushed once.
#   3. NuGet restores into <directory>/nuget/packages and caches its downloads
#      in <directory>/nuget/http-cache (NUGET_PACKAGES, NUGET_HTTP_CACHE_PATH),
#      both empty: the package comes from the server, not from a folder an
#      earlier restore filled. The configure of the preset md restores WinXL
#      through the provider, and the build makes core-only and demo.
#   4. The restored WinXL must be the version of the package, taken from the
#      server (.nupkg.metadata of the restore names its source), and
#      find_package(wxl) must have taken it; core-only prints the version, and
#      the window of demo opens on the Windows App Runtime winxl of the package
#      installs, as the job consumer of .github/workflows/package.yml checks.
#
# NuGet refuses a source over plain HTTP. A server on this machine, at
# localhost, 127.0.0.1 or [::1], is allowed one (allowInsecureConnections in
# the nuget.config of the copy); any other HTTP source stops the script.

[CmdletBinding()]
param(
    [Parameter(Mandatory)] [string]$Package,
    [Parameter(Mandatory)] [string]$Source,
    [Parameter(Mandatory)] [string]$Application,
    [switch]$NoPush,
    [ValidateSet('Debug', 'Release')] [string]$Config = 'Debug',
    # Parallel jobs of the build; 0 leaves the number to Ninja.
    [int]$Jobs = 0
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$nugetOrg = 'https://api.nuget.org/v3/index.json'

if (-not $env:VSINSTALLDIR) {
    throw 'No Visual Studio developer environment (VSINSTALLDIR is not set): run from a Developer PowerShell.'
}
if (-not $NoPush -and -not $env:WXL_NUGET_API_KEY) {
    throw 'The API key of the server is not in WXL_NUGET_API_KEY.'
}

$Package = (Resolve-Path $Package).Path
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = [IO.Compression.ZipFile]::OpenRead($Package)
try {
    $entries = @($zip.Entries | Where-Object { $_.FullName -notmatch '/' -and $_.Name -like '*.nuspec' })
    if ($entries.Count -ne 1) { throw "$Package has $($entries.Count) manifests at its root, not one." }
    $reader = [IO.StreamReader]::new($entries[0].Open())
    try { [xml]$nuspec = $reader.ReadToEnd() } finally { $reader.Dispose() }
} finally { $zip.Dispose() }
$id = $nuspec.package.metadata.id
$version = $nuspec.package.metadata.version
if ($id -ne 'WinXL') { throw "$Package is the package $id, not WinXL." }

$uri = [Uri]$Source
$insecure = $false
if ($uri.Scheme -eq 'http') {
    if (-not $uri.IsLoopback) {
        throw "$Source is plain HTTP and not on this machine: a server elsewhere is given by HTTPS."
    }
    $insecure = $true
} elseif ($uri.Scheme -ne 'https') {
    throw "$Source is not the service index of a NuGet server (http or https)."
}

# 1. The application.
if ((Test-Path $Application) -and (Get-ChildItem -Force $Application | Select-Object -First 1)) {
    throw "$Application is not empty: the check builds in a new copy of the package consumer."
}
New-Item -ItemType Directory -Force $Application | Out-Null
$Application = (Resolve-Path $Application).Path
Copy-Item (Join-Path $root 'tests/package-consumer/*') $Application -Recurse
New-Item -ItemType Directory (Join-Path $Application 'cmake') | Out-Null
Copy-Item (Join-Path $root 'cmake/winxl-provider.cmake') (Join-Path $Application 'cmake')

$sourceKey = 'server'
if ($Source -eq $nugetOrg) {
    $sources = "    <add key=`"$sourceKey`" value=`"$Source`" protocolVersion=`"3`" />"
    $mapping = "    <packageSource key=`"$sourceKey`"><package pattern=`"*`" /></packageSource>"
} else {
    $allow = if ($insecure) { ' allowInsecureConnections="true"' } else { '' }
    $sources = "    <add key=`"$sourceKey`" value=`"$Source`" protocolVersion=`"3`"$allow />`n" +
               "    <add key=`"nuget.org`" value=`"$nugetOrg`" protocolVersion=`"3`" />"
    $mapping = "    <packageSource key=`"$sourceKey`"><package pattern=`"WinXL`" /></packageSource>`n" +
               "    <packageSource key=`"nuget.org`"><package pattern=`"*`" /></packageSource>"
}
$nugetConfig = Join-Path $Application 'nuget.config'
Set-Content $nugetConfig -Encoding utf8 -Value @"
<?xml version="1.0" encoding="utf-8"?>
<!-- Written by tools/package/server-check.ps1: WinXL from the server, everything else from nuget.org. -->
<configuration>
  <packageSources>
    <clear />
$sources
  </packageSources>
  <packageSourceMapping>
$mapping
  </packageSourceMapping>
</configuration>
"@

# 2. The push. global.json beside this script asks for a released SDK.
if (-not $NoPush) {
    Push-Location $PSScriptRoot
    try {
        dotnet nuget push $Package --source $sourceKey --configfile $nugetConfig --api-key $env:WXL_NUGET_API_KEY --timeout 1200
        if ($LASTEXITCODE -ne 0) { throw "dotnet nuget push exited with $LASTEXITCODE" }
    } finally { Pop-Location }
}

# 3. The restore, the configure and the build, with folders of NuGet of their own.
# The variables are given back as they were: the script runs in the caller's shell.
$packages = Join-Path $Application 'nuget/packages'
$saved = @{}
foreach ($name in 'NUGET_PACKAGES', 'NUGET_HTTP_CACHE_PATH', 'RestoreLockedMode') {
    $saved[$name] = [Environment]::GetEnvironmentVariable($name)
}
$env:NUGET_PACKAGES = $packages
$env:NUGET_HTTP_CACHE_PATH = Join-Path $Application 'nuget/http-cache'
$env:RestoreLockedMode = $null
Push-Location $Application
try {
    cmake --preset md
    if ($LASTEXITCODE -ne 0) { throw "the configure exited with $LASTEXITCODE" }
    $build = @('--build', '--preset', "md-$($Config.ToLowerInvariant())")
    if ($Jobs -gt 0) { $build += @('-j', $Jobs) }
    cmake @build
    if ($LASTEXITCODE -ne 0) { throw "the build exited with $LASTEXITCODE" }

    # 4. What the restore took and from where.
    $restored = Join-Path $packages "winxl/$($version.ToLowerInvariant())"
    $metadata = Join-Path $restored '.nupkg.metadata'
    if (-not (Test-Path $metadata)) { throw "the restore put no WinXL $version into $packages" }
    $from = (Get-Content $metadata -Raw | ConvertFrom-Json).source
    if ($from -ne $Source) { throw "the restore took WinXL $version from $from, not from $Source" }
    "WinXL $version restored from $from"

    $dir = (Select-String -Path 'build/md/CMakeCache.txt' -Pattern '^wxl_DIR:[A-Z]+=(.*)$').Matches[0].Groups[1].Value
    $expectedDir = (Join-Path $restored 'build/native/lib/cmake/wxl').Replace('\', '/')
    if ($dir -ne $expectedDir) { throw "find_package(wxl) took $dir, not $expectedDir" }
    "wxl_DIR $dir"

    $line = & "build/md/$Config/core-only.exe"
    if ($LASTEXITCODE -ne 0) { throw "core-only exited with $LASTEXITCODE" }
    "core-only: $line"
    if ($line -ne "wxl ${version}: 9 code units") { throw "core-only printed '$line', not the version $version" }

    & (Join-Path $restored 'build/native/bin/winxl.exe') runtime install --packages 'build/md/wxl.packages/restored-packages.json'
    if ($LASTEXITCODE -ne 0) { throw "winxl runtime install exited with $LASTEXITCODE" }
    $exe = Get-Item "build/md/$Config/demo.exe"
    $p = Start-Process -FilePath $exe.FullName -WorkingDirectory $exe.DirectoryName -PassThru
    Start-Sleep -Seconds 20
    $p.Refresh()
    if ($p.HasExited) { throw "demo exited within 20 s, code $($p.ExitCode)" }
    $window = $p.MainWindowHandle
    "demo: running after 20 s, main window '$($p.MainWindowTitle)'"
    Stop-Process -Id $p.Id
    if ($window -eq 0) { throw 'demo runs but shows no main window' }
} finally {
    Pop-Location
    foreach ($name in $saved.Keys) { [Environment]::SetEnvironmentVariable($name, $saved[$name]) }
}
