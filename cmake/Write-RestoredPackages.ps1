# Writes the list of the restored packages that winui-srcgen takes with --packages, for a
# .vcxproj that references the package WinXL: WinXL.targets runs it on the restore of the
# project, where a build on CMake has the list cmake/wxl_restored_packages.cmake writes. The
# list is the same text that file writes from the same restore -- its format is described
# there -- and the test restored-packages-msbuild compares the two on the restore of the tree.
#
# Usage: powershell.exe -NoProfile -File Write-RestoredPackages.ps1 -Assets <project.assets.json>
#            -WindowsSdk <version> -Metadata <file> -Out <list>
#
#   -Assets      obj/project.assets.json of the restore: every package it names, with the
#                directory it is in among the package folders
#   -WindowsSdk  the version of the Windows SDK the projection of the platform is made from
#   -Metadata    a file of the .winmd files of that projection, one per line, in the order of
#                the -in entries of the response file of the platform projection
#   -Out         the list, written only when its text changes, so that a build that changes
#                nothing touches no file
#
# Prints, one per line, <id>|<directory> of every package of the list: WinXL.targets takes the
# files it puts beside the program and the include directories of the profile layer from them.
#
# Compatible with Windows PowerShell 5.1: no `?.`, `??`, ternary, `&&`.

param(
    [Parameter(Mandatory=$true)] [string]$Assets,
    [Parameter(Mandatory=$true)] [string]$WindowsSdk,
    [Parameter(Mandatory=$true)] [string]$Metadata,
    [Parameter(Mandatory=$true)] [string]$Out
)

$ErrorActionPreference = 'Stop'

# Text as a JSON string, quotes included, as the list of CMake writes it: a \ and a " escaped,
# and a control character, which neither a Windows path nor a NuGet id or version holds, refused.
function ConvertTo-JsonString([string]$Text) {
    if ($Text -match '[\x01-\x1f]') {
        throw "`"$Text`" has a control character, which the list of the restore does not write."
    }
    return '"' + $Text.Replace('\', '\\').Replace('"', '\"') + '"'
}

if (-not (Test-Path -LiteralPath $Assets -PathType Leaf)) { throw "The restore wrote no $Assets." }
$json = ConvertFrom-Json -InputObject ([IO.File]::ReadAllText($Assets, [Text.Encoding]::UTF8))
if (-not $json.packageFolders -or -not $json.libraries) {
    throw "$Assets has no object `"packageFolders`" or no object `"libraries`"."
}
$folders = @($json.packageFolders.PSObject.Properties | ForEach-Object { $_.Name.Replace('\', '/') })

# A member of "libraries" is named <id>/<version>, and its "path" is the directory of the
# package relative to one of the package folders.
$packages = @{}
foreach ($library in $json.libraries.PSObject.Properties) {
    $key = $library.Name
    $entry = $library.Value
    if ($entry.type -eq 'project') { continue }
    if ($entry.type -ne 'package') {
        throw "${Assets}: the entry `"$key`" is of type `"$($entry.type)`", neither a package nor a project."
    }
    $slash = $key.IndexOf('/')
    if (-not $entry.path -or $slash -lt 1) {
        throw "${Assets}: the package `"$key`" has no path, or its name is not <id>/<version>."
    }
    $id = $key.Substring(0, $slash)
    $version = $key.Substring($slash + 1)
    $dir = ''
    foreach ($folder in $folders) {
        $root = $folder
        if (-not $root.EndsWith('/')) { $root += '/' }
        $candidate = $root + $entry.path
        if (Test-Path -LiteralPath $candidate -PathType Container) {
            $dir = $candidate
            break
        }
    }
    if (-not $dir) {
        throw "$Assets names $key at $($entry.path), which is in none of its package folders ($($folders -join ', '))."
    }
    $packages[$id.ToLowerInvariant()] = @{ id = $id; version = $version; directory = $dir }
}
if ($packages.Count -eq 0) { throw "$Assets names no package." }

$winmds = @()
foreach ($line in [IO.File]::ReadAllLines($Metadata)) {
    if ($line.Trim() -eq '') { continue }
    $winmd = $line.Trim().Replace('\', '/')
    if (-not $winmd.ToLowerInvariant().EndsWith('.winmd') -or -not (Test-Path -LiteralPath $winmd -PathType Leaf)) {
        throw "$Metadata names $winmd, which is not a .winmd file on disk."
    }
    $winmds += '            ' + (ConvertTo-JsonString $winmd)
}
if ($winmds.Count -eq 0) { throw "$Metadata names no Windows SDK metadata." }

# By the id in lower case, ordinally, as CMake sorts them.
$keys = [string[]]@($packages.Keys)
[Array]::Sort($keys, [StringComparer]::Ordinal)
$entries = @()
foreach ($key in $keys) {
    $package = $packages[$key]
    $entries += '        { "id": ' + (ConvertTo-JsonString $package.id) + ', "version": ' +
        (ConvertTo-JsonString $package.version) + ', "directory": ' + (ConvertTo-JsonString $package.directory) + ' }'
}

$text = "{`n" +
    "    `"packages`": [`n" + ($entries -join ",`n") + "`n    ],`n" +
    "    `"windowsSdk`": {`n" +
    "        `"version`": " + (ConvertTo-JsonString $WindowsSdk) + ",`n" +
    "        `"metadata`": [`n" + ($winmds -join ",`n") + "`n        ]`n" +
    "    }`n" +
    "}`n"

$written = ''
if (Test-Path -LiteralPath $Out -PathType Leaf) { $written = [IO.File]::ReadAllText($Out) }
if ($written -ne $text) {
    $parent = Split-Path -Parent $Out
    if ($parent -and -not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory $parent | Out-Null }
    [IO.File]::WriteAllText($Out, $text, (New-Object Text.UTF8Encoding $false))
}

foreach ($key in $keys) {
    Write-Output ($packages[$key].id + '|' + $packages[$key].directory)
}
