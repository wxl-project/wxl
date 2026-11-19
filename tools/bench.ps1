# Builds the benchmarks of the tree and runs every one of them, writing what
# they print and what the machine was into a folder of its own.
#
#   pwsh -NoProfile -File tools\bench.ps1
#   pwsh -NoProfile -File tools\bench.ps1 -Toolset 14.52
#
# The build goes through tools\build.ps1, into build\bench and in Release: the
# benchmarks are switched on there (WXL_BUILD_BENCHMARKS) and the documentation
# site off (WXL_BUILD_DOCS), and build\x64-dynamic is not touched. Which
# benchmarks there are, with what arguments and on what input files, CMake says: wxl_add_benchmark() registers each one, and the configure
# writes the list into build\bench\benchmarks-Release.json. Nothing here names a
# benchmark.
#
# The results go to build\bench\results\<date>-<time>-<commit>\: one text file
# per benchmark, with everything it printed, and stand.txt -- the commit and
# whether the tree had changes, the compiler, the processor, the OS build, the
# power scheme, and for each run its arguments, how long it took, how it exited
# and how busy the machine was just before it started. The folder is outside
# git; the numbers that are kept go to the result pages by hand.
#
# A benchmark that fails -- a missing input, an exit code other than 0 -- is
# written down as such and the others run on; the script then ends with exit
# code 1.
[CmdletBinding()]
param(
    # The MSVC toolset, passed on to build.ps1.
    [string]$Toolset = '14.53'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $root 'build\bench'
$config = 'Release'

# ---- Build ----
#
# In this process: build.ps1 imports the Visual Studio environment into it, and
# the compiler banner below is asked of the same toolset.
& (Join-Path $PSScriptRoot 'build.ps1') -Config $config -BuildDir $buildDir -Toolset $Toolset `
    -Target wxl.benchmarks -Define 'WXL_BUILD_BENCHMARKS=ON', 'WXL_BUILD_DOCS=OFF'
if ($LASTEXITCODE -ne 0) { throw "The benchmarks did not build (exit code $LASTEXITCODE)." }

$listFile = Join-Path $buildDir "benchmarks-$config.json"
if (-not (Test-Path $listFile)) { throw "No list of benchmarks: $listFile" }
$benchmarks = @(Get-Content $listFile -Raw | ConvertFrom-Json)

# ---- The stand ----

$commit = (git -C $root rev-parse HEAD).Trim()
$short = (git -C $root rev-parse --short HEAD).Trim()
$changes = @(git -C $root status --porcelain)

# The compiler the build used, as CMake found it, and its own banner: run with
# nothing to compile, cl writes its name and version first and the usage line
# after them.
$compiler = (Select-String -Path (Join-Path $buildDir 'CMakeCache.txt') -Pattern '^CMAKE_CXX_COMPILER:\w+=(.*)$').Matches[0].Groups[1].Value
$banner = (& $compiler 2>&1 | ForEach-Object { "$_" } | Where-Object { $_ -match 'Version' } | Select-Object -First 1)

$cpu = Get-CimInstance Win32_Processor
$os = Get-CimInstance Win32_OperatingSystem
$ubr = (Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion').UBR
$power = (powercfg /getactivescheme | Out-String).Trim()

# How busy the whole machine is: the load of every processor, each averaged by
# Windows over the last second, and their mean. Asked a second after the
# previous benchmark has ended, so that the second it covers is not that
# benchmark's own.
function Get-MachineLoad {
    Start-Sleep -Seconds 1
    $loads = @(Get-CimInstance Win32_Processor | ForEach-Object { $_.LoadPercentage })
    [math]::Round(($loads | Measure-Object -Average).Average, 1)
}

# Numbers in the files with a decimal point, whatever the locale of the machine.
$invariant = [System.Globalization.CultureInfo]::InvariantCulture

$started = Get-Date
$results = Join-Path $buildDir ("results\{0}-{1}" -f $started.ToString('yyyy-MM-dd-HHmmss'), $short)
New-Item -ItemType Directory -Force $results | Out-Null

$stand = [System.Collections.Generic.List[string]]::new()
$stand.Add("Started:      $($started.ToString('yyyy-MM-dd HH:mm:ss zzz'))")
$stand.Add("Commit:       $commit" + $(if ($changes.Count) { " (with uncommitted changes, $($changes.Count) paths)" } else { ' (clean)' }))
$stand.Add("Build:        $config, $buildDir")
$stand.Add("Compiler:     $compiler")
$stand.Add("              $banner")
foreach ($p in $cpu) {
    $stand.Add("Processor:    $($p.Name.Trim()), $($p.NumberOfCores) cores, $($p.NumberOfLogicalProcessors) logical processors")
}
$stand.Add("OS:           $($os.Caption) $($os.Version).$ubr")
$stand.Add("Power scheme: $power")
$stand.Add('')
$stand.Add('Runs (load: the machine''s processor load just before the run):')

# ---- Runs ----

[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
$failed = 0

Push-Location $results
try {
    foreach ($b in $benchmarks) {
        $arguments = @($b.arguments)
        $inputs = @($b.inputs)
        $output = Join-Path $results "$($b.target).txt"

        $stand.Add('')
        $stand.Add("$($b.target)")
        $stand.Add("  arguments: $($arguments -join ' ')")
        if ($inputs.Count) { $stand.Add("  inputs:    $($inputs -join ', ')") }

        $missing = @($inputs | Where-Object { -not (Test-Path $_) })
        if ($missing.Count) {
            $stand.Add("  not run: missing input $($missing -join ', ')")
            "not run: missing input $($missing -join ', ')" | Set-Content -Encoding utf8 $output
            $failed++
            continue
        }

        $load = Get-MachineLoad
        "== $($b.target) $($arguments -join ' ')"
        $watch = [System.Diagnostics.Stopwatch]::StartNew()
        & $b.executable @arguments *>&1 | ForEach-Object { "$_" } | Set-Content -Encoding utf8 $output
        $code = $LASTEXITCODE
        $watch.Stop()

        $seconds = $watch.Elapsed.TotalSeconds.ToString('F1', $invariant)
        $stand.Add("  load:      $($load.ToString($invariant)) %")
        $stand.Add("  duration:  $seconds s")
        $stand.Add("  exit code: $code")
        "   exit code $code, $seconds s"
        if ($code -ne 0) { $failed++ }
    }
} finally {
    Pop-Location
    $stand | Set-Content -Encoding utf8 (Join-Path $results 'stand.txt')
}

"Results: $results"
if ($failed) {
    "$failed of $($benchmarks.Count) benchmarks failed or did not run"
    exit 1
}
