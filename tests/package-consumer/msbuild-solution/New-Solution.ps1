# The scenario of an application on MSBuild made with winxl: a solution of kind wxl with the
# program Shop on the profile project wxl, the library model on wxl.core alone and the library
# views on the profile, which uses model. The solution and its projects are what winxl writes; the
# sources beside this script replace those winxl starts each project with, the profile among them,
# which selects the component items the list of views needs, and nuget.config of the
# directory above names the sources of the restore: WinXL from feed\ of the solution, which the
# caller fills, the rest from nuget.org.
#
#     New-Solution.ps1 -Winxl <winxl.exe of the package> -Directory <new directory>
#
# Then, from a developer prompt in <directory>, with the package published into <directory>\feed:
#
#     msbuild Shop.slnx -restore -p:Configuration=Debug -p:Platform=x64 -p:VCToolsVersion=%VCToolsVersion%

param(
    [Parameter(Mandatory)] [string]$Winxl,
    [Parameter(Mandatory)] [string]$Directory
)

$ErrorActionPreference = 'Stop'

function Invoke-Winxl {
    & $Winxl @args
    if ($LASTEXITCODE -ne 0) { throw "winxl $($args -join ' ') exited with $LASTEXITCODE" }
}

$Directory = $PSCmdlet.GetUnresolvedProviderPathFromPSPath($Directory)
Invoke-Winxl new wxl --name Shop --dir $Directory
Push-Location $Directory
try {
    Invoke-Winxl add library --name model
    Invoke-Winxl add library --name views --ui --references model
} finally {
    Pop-Location
}

# Each source replaces the file winxl wrote at the same place: a source without one means the
# layout winxl writes is not the one the scenario stands on. The scripts beside this one are not
# sources: the sources are in the directories of the projects.
$sources = Get-ChildItem $PSScriptRoot -Directory | Get-ChildItem -Recurse -File
foreach ($source in $sources) {
    $relative = $source.FullName.Substring($PSScriptRoot.Length + 1)
    $target = Join-Path $Directory $relative
    if (-not (Test-Path $target)) { throw "winxl wrote no $relative, which $($source.FullName) replaces" }
    Copy-Item $source.FullName $target -Force
    "replaced $relative"
}
Copy-Item (Join-Path $PSScriptRoot '..\nuget.config') $Directory
