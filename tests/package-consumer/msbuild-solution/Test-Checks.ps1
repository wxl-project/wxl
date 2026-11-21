# The refusals of winxl new and winxl add library, and the profile checks of WinXL.targets, each
# expected to stop with its own message: a solution winxl of the package writes into <Directory>,
# changed the way each case needs. Nothing is compiled -- the checks of WinXL.targets run before the
# wrappers and the compile, and each MSBuild case builds only the target _WinXLCheckProfiles of one
# project, without building its references. The checks of the version of WinXL restore the program
# once, for the project.assets.json winxl reads.
#
#     Test-Checks.ps1 -Winxl <winxl.exe of the package> -Directory <new directory> -Feed <local feed>
#
# From a developer prompt (MSBuild on PATH, VCToolsVersion set), with the package published into
# <Feed>; nuget.config of the directory above, with <Feed> as its local source, is put beside the
# solution.

param(
    [Parameter(Mandatory)] [string]$Winxl,
    [Parameter(Mandatory)] [string]$Directory,
    [Parameter(Mandatory)] [string]$Feed
)

$ErrorActionPreference = 'Stop'
$Directory = $PSCmdlet.GetUnresolvedProviderPathFromPSPath($Directory)
$Feed = $PSCmdlet.GetUnresolvedProviderPathFromPSPath($Feed)
if (-not $env:VCToolsVersion) { throw 'VCToolsVersion is not set: run from a developer prompt' }
$failures = [System.Collections.Generic.List[string]]::new()

# winxl with <Arguments> in the current directory: its exit code and what it wrote to stderr.
function Invoke-Winxl([string[]]$Arguments) {
    $ErrorActionPreference = 'Continue'
    $errors = @(& $Winxl @Arguments 2>&1 | Where-Object { $_ -is [System.Management.Automation.ErrorRecord] } | ForEach-Object { "$_" })
    [pscustomobject]@{ Code = $LASTEXITCODE; Message = $errors -join ' ' }
}

function Assert-Winxl([string]$What, [int]$Code, [string]$Message, [string[]]$Arguments) {
    $result = Invoke-Winxl $Arguments
    if ($result.Code -ne $Code -or -not $result.Message.Contains($Message)) {
        $failures.Add("$What`: exit $($result.Code), '$($result.Message)'; expected exit $Code and '$Message'")
    }
    "{0,-46} exit {1}" -f $What, $result.Code
}

# MSBuild with the target _WinXLCheckProfiles of <Project>, which has to stop with an error that
# holds <Message> and not <Absent>, or pass when <Message> is empty.
function Assert-Profiles([string]$What, [string]$Project, [string]$Message, [string]$Absent) {
    $ErrorActionPreference = 'Continue'
    $output = @(msbuild $Project -restore -nologo -verbosity:minimal -p:Configuration=Debug -p:Platform=x64 `
        "-p:VCToolsVersion=$env:VCToolsVersion" -p:BuildProjectReferences=false -t:_WinXLCheckProfiles 2>&1 | ForEach-Object { "$_" })
    $code = $LASTEXITCODE
    $errors = @($output | Where-Object { $_ -match ': error ' })
    if ($Message) {
        if ($code -eq 0 -or -not ($errors | Where-Object { $_.Contains($Message) -and -not ($Absent -and $_.Contains($Absent)) })) {
            $failures.Add("$What`: exit $code, expected an error with '$Message'; errors: $($errors -join ' | ')")
        }
    } elseif ($code -ne 0) {
        $failures.Add("$What`: exit $code, expected no error; errors: $($errors -join ' | ')")
    }
    "{0,-46} exit {1}" -f $What, $code
}

# The project file <Project> without its ProjectReference to <Include>, or with one more.
function Remove-Reference([string]$Project, [string]$Include) {
    $text = [IO.File]::ReadAllText($Project)
    $pattern = '(?s)\s*<ProjectReference Include="' + [regex]::Escape($Include) + '">.*?</ProjectReference>'
    if ($text -notmatch $pattern) { throw "$Project has no ProjectReference to $Include" }
    [IO.File]::WriteAllText($Project, [regex]::Replace($text, $pattern, ''))
}
function Add-Reference([string]$Project, [string]$Include, [string]$Guid) {
    $text = [IO.File]::ReadAllText($Project)
    $reference = "<ProjectReference Include=`"$Include`">`r`n      <Project>$Guid</Project>`r`n    </ProjectReference>`r`n    "
    $at = $text.IndexOf('<ProjectReference ')
    if ($at -lt 0) { throw "$Project has no ProjectReference to add one beside" }
    [IO.File]::WriteAllText($Project, $text.Insert($at, $reference))
}

if (Test-Path $Directory) { throw "$Directory is there already" }

# ---- winxl ------------------------------------------------------------------------------------
"== winxl"
New-Item -ItemType Directory $Directory | Out-Null
Push-Location $Directory
try {
    Assert-Winxl 'new: the name of the profile project' 1 'is the name of the profile project' @('new', 'wxl', '--name', 'Wxl')
    Assert-Winxl 'new: a profile that extends itself' 1 'would extend itself' @('new', 'wxl', '--name', 'Base')
    Assert-Winxl 'new: a keyword of C++' 1 'keyword of C++' @('new', 'wxl', '--name', 'class')
    Assert-Winxl 'new: a device of Windows' 1 'device of Windows' @('new', 'wxl', '--name', 'nul')
    Assert-Winxl 'new: a macro of Windows' 1 'is a macro' @('new', 'wxl', '--name', 'small')
    Assert-Winxl 'new: a character no path has' 1 'is not a path' @('new', 'wxl', '--name', 'A', '--dir', 'a*b')
    Assert-Winxl 'new: relative to a drive' 1 'current directory of the drive' @('new', 'wxl', '--name', 'A', '--dir', 'a:b')
    Assert-Winxl 'new P' 0 '' @('new', 'wxl', '--name', 'P')
} finally {
    Pop-Location
}
$solution = Join-Path $Directory 'P'
$program = Join-Path $solution 'P\P.vcxproj'
Copy-Item (Join-Path $PSScriptRoot '..\nuget.config') $solution
(Get-Content (Join-Path $solution 'nuget.config') -Raw).Replace('value="feed"', "value=`"$Feed`"") |
    Set-Content (Join-Path $solution 'nuget.config') -NoNewline
Push-Location $solution
try {
    Assert-Winxl 'add: a header of the package' 1 'ui.h' @('add', 'library', '--name', 'ui')
    Assert-Winxl 'add: a header of the Windows SDK' 1 'winbase.h' @('add', 'library', '--name', 'winbase')
    Assert-Winxl 'add: a header of the CRT' 1 'math.h' @('add', 'library', '--name', 'math')
    Assert-Winxl 'add: a function macro of Windows' 1 'is a macro' @('add', 'library', '--name', 'GetWindowText')
    Assert-Winxl 'add: a macro of the CRT' 1 'is a macro' @('add', 'library', '--name', 'errno')

    $saved = [IO.File]::ReadAllText($program)
    [IO.File]::WriteAllText($program, [regex]::Replace($saved, '(<PackageReference Include="WinXL" Version=")[^"]+', '${1}9.9.9'))
    Assert-Winxl 'add: another version of WinXL' 2 'run the winxl of the package WinXL 9.9.9' @('add', 'library', '--name', 'late')

    # The version NuGet restored decides once there is a restore of the reference as it is: the
    # minimum 0.1.0 resolves to the version of the feed, the one of this winxl.
    $own = (& $Winxl --version).Trim()
    $slnx = Join-Path $solution 'P.slnx'
    $savedSolution = [IO.File]::ReadAllBytes($slnx)
    $assets = Join-Path $solution 'P\obj\project.assets.json'
    [IO.File]::WriteAllText($program, [regex]::Replace($saved, '(<PackageReference Include="WinXL" Version=")[^"]+', '${1}0.1.0'))
    $ErrorActionPreference = 'Continue'
    $restore = @(msbuild $program -t:Restore -nologo -verbosity:minimal -p:Configuration=Debug -p:Platform=x64 2>&1 | ForEach-Object { "$_" })
    $ErrorActionPreference = 'Stop'
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path $assets)) { throw "the restore of $program failed: $($restore -join ' | ')" }
    Assert-Winxl 'add: WinXL 0.1.0 restored as this winxl' 0 '' @('add', 'library', '--name', 'early')
    if (-not ([IO.File]::ReadAllText((Join-Path $solution 'early\early.vcxproj'))).Contains("<PackageReference Include=`"WinXL`" Version=`"$own`" />")) {
        $failures.Add("the library added on the restore of WinXL 0.1.0 does not reference WinXL $own")
    }
    Remove-Item -LiteralPath (Join-Path $solution 'early') -Recurse -Force
    [IO.File]::WriteAllBytes($slnx, $savedSolution)
    [IO.File]::WriteAllText($program, [regex]::Replace($saved, '(<PackageReference Include="WinXL" Version=")[^"]+', '${1}0.1.0'))
    $restored = [IO.File]::ReadAllText($assets)
    [IO.File]::WriteAllText($assets, $restored.Replace("WinXL/$own", 'WinXL/9.9.9').Replace("winxl/$own", 'winxl/9.9.9'))
    Assert-Winxl 'add: another version of WinXL restored' 2 'run the winxl of the package WinXL 9.9.9, which the restore put at' @('add', 'library', '--name', 'late')
    [IO.File]::WriteAllText($assets, '{ "version": 3, ')
    Assert-Winxl 'add: a restore that is not JSON' 2 'is not JSON' @('add', 'library', '--name', 'late')
    [IO.File]::WriteAllText($assets, $restored)
    [IO.File]::WriteAllText($program, [regex]::Replace($saved, '(<PackageReference Include="WinXL" Version=")[^"]+', '${1}9.9.9'))
    Assert-Winxl 'add: a reference changed since the restore' 2 'WinXL 9.9.9, which NuGet has not restored' @('add', 'library', '--name', 'late')
    [IO.File]::WriteAllText($program, [regex]::Replace($saved, '(<PackageReference Include="WinXL" Version=")[^"]+', '${1}0.1.0'))
    Remove-Item -LiteralPath (Join-Path $solution 'P\obj') -Recurse -Force
    Assert-Winxl 'add: WinXL 0.1.0 not restored' 2 'WinXL 0.1.0, which NuGet has not restored' @('add', 'library', '--name', 'late')
    [IO.File]::WriteAllText($program, $saved)

    # A write refused: the solution and the program as they were, no directory of the library.
    $slnx = Join-Path $solution 'P.slnx'
    $before = [IO.File]::ReadAllBytes($slnx)
    Set-ItemProperty $program -Name IsReadOnly -Value $true
    Assert-Winxl 'add: the program cannot be written' 3 'cannot be written' @('add', 'library', '--name', 'refused')
    Set-ItemProperty $program -Name IsReadOnly -Value $false
    if (Test-Path (Join-Path $solution 'refused')) { $failures.Add('a refused add left its directory') }
    if (-not [Linq.Enumerable]::SequenceEqual([byte[]]$before, [IO.File]::ReadAllBytes($slnx))) { $failures.Add('a refused add left the solution changed') }

    # The form of a file winxl changes: a solution with a byte order mark and the line ends of Unix.
    $text = [IO.File]::ReadAllText($slnx).Replace("`r`n", "`n")
    [IO.File]::WriteAllText($slnx, $text, [Text.UTF8Encoding]::new($true))
    Assert-Winxl 'add views --ui' 0 '' @('add', 'library', '--name', 'views', '--ui')
    $bytes = [IO.File]::ReadAllBytes($slnx)
    $after = [Text.Encoding]::UTF8.GetString($bytes)
    if ($bytes[0] -ne 0xEF -or $after.Contains("`r`n") -or -not $after.EndsWith("`n") -or -not $after.Contains('views/views.vcxproj')) {
        $failures.Add('add library changed the form of the solution file')
    }
    Assert-Winxl 'add A --references views' 0 '' @('add', 'library', '--name', 'A', '--references', 'views')
    Assert-Winxl 'add C --references views' 0 '' @('add', 'library', '--name', 'C', '--references', 'views')
} finally {
    Pop-Location
}

# ---- WinXL.targets ----------------------------------------------------------------------------
"== WinXL.targets"
$profileProject = Join-Path $solution 'wxl\wxl.vcxproj'
Assert-Profiles 'the program as winxl writes it' $program ''

# A second profile project naming the same profile, and then a profile of its own.
$second = Join-Path $solution 'wxl2'
New-Item -ItemType Directory $second | Out-Null
$secondGuid = "{$([guid]::NewGuid().ToString().ToUpperInvariant())}"
$secondText = [IO.File]::ReadAllText($profileProject)
$secondText = [regex]::Replace($secondText, '<ProjectGuid>\{[^}]+\}</ProjectGuid>', "<ProjectGuid>$secondGuid</ProjectGuid>")
$secondText = $secondText.Replace('<WinXLProfile>P.winxl-profile</WinXLProfile>', '<WinXLProfile>..\wxl\P.winxl-profile</WinXLProfile>')
[IO.File]::WriteAllText((Join-Path $second 'wxl2.vcxproj'), $secondText)
$programText = [IO.File]::ReadAllText($program)
Add-Reference $program '..\wxl2\wxl2.vcxproj' $secondGuid
Assert-Profiles 'two profile projects, one profile' $program 'stands on the profile projects'
Copy-Item (Join-Path $solution 'wxl\P.winxl-profile') (Join-Path $second 'Other.winxl-profile')
[IO.File]::WriteAllText((Join-Path $second 'wxl2.vcxproj'), $secondText.Replace('..\wxl\P.winxl-profile', 'Other.winxl-profile'))
Assert-Profiles 'two profile projects, two profiles' $program 'stands on the profile projects'
[IO.File]::WriteAllText($program, $programText)

# The profile project reached through libraries alone; the error names the library referenced.
Remove-Reference $program '..\wxl\wxl.vcxproj'
Remove-Reference $program '..\A\A.vcxproj'
Remove-Reference $program '..\C\C.vcxproj'
Assert-Profiles 'a program through a library' $program "through $(Join-Path $solution 'views\views.vcxproj') and does not reference it"
[IO.File]::WriteAllText($program, $programText)
Remove-Reference $program '..\wxl\wxl.vcxproj'
Remove-Reference $program '..\views\views.vcxproj'
Remove-Reference $program '..\C\C.vcxproj'
Remove-Reference (Join-Path $solution 'A\A.vcxproj') '..\wxl\wxl.vcxproj'
Assert-Profiles 'a program through two libraries' $program "through $(Join-Path $solution 'A\A.vcxproj') and does not reference it" (Join-Path $solution 'views\views.vcxproj')
[IO.File]::WriteAllText($program, $programText)
Remove-Reference (Join-Path $solution 'C\C.vcxproj') '..\wxl\wxl.vcxproj'
Assert-Profiles 'a library through a library' (Join-Path $solution 'C\C.vcxproj') "through $(Join-Path $solution 'views\views.vcxproj') and does not reference it"

if ($failures.Count -gt 0) {
    $failures | ForEach-Object { "FAILED: $_" }
    throw "$($failures.Count) of the checks failed"
}
'every check stopped as expected'
