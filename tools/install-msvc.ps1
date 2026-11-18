[CmdletBinding()]
param(
	[string]$InstallPath
)

$ErrorActionPreference = 'Stop'
$minimumToolset = [version]'14.53.36911'
$component = 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64'

function Get-VisualStudioPathFromToolset([string]$path) {
	if (-not $path) { return $null }

	$fullPath = [IO.Path]::GetFullPath($path).TrimEnd('\')
	if ($fullPath -match '^(?<install>.+)\\VC\\Tools\\MSVC\\[^\\]+$') {
		return $Matches.install
	}

	return $null
}

if ($InstallPath) {
	$installationPath = [IO.Path]::GetFullPath($InstallPath)
} else {
	$installationPath = Get-VisualStudioPathFromToolset $env:VCToolsInstallDir

	if (-not $installationPath) {
		$compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
		if ($compiler) {
			$installationPath = Get-VisualStudioPathFromToolset (Split-Path (Split-Path (Split-Path (Split-Path $compiler.Source -Parent) -Parent) -Parent) -Parent)
		}
	}

	if (-not $installationPath) {
		$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
		if (-not (Test-Path -LiteralPath $vswhere)) {
			throw "Visual Studio discovery tool was not found: $vswhere. Install the Visual Studio Installer or pass -InstallPath."
		}

		$instances = @(& $vswhere -prerelease -products '*' -latest `
			-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
			-property installationPath)
		if ($LASTEXITCODE -ne 0) { throw "vswhere.exe exited with code $LASTEXITCODE." }
		if (-not $instances) { throw 'No Visual Studio installation with the MSVC C++ tools was found.' }
		$installationPath = $instances[0].Trim()
	}
}

$developerCommand = Join-Path $installationPath 'Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $developerCommand)) {
	throw "Visual Studio developer tools were not found in '$installationPath'. Pass -InstallPath for the installation that contains the C++ workload."
}

$toolsetsPath = Join-Path $installationPath 'VC\Tools\MSVC'
$installedToolsets = @()
if (Test-Path -LiteralPath $toolsetsPath) {
	$installedToolsets = @(Get-ChildItem -LiteralPath $toolsetsPath -Directory | Where-Object {
		$version = $null
		[version]::TryParse($_.Name, [ref]$version) -and $version -ge $minimumToolset
	})
}

if ($installedToolsets) {
	$version = ($installedToolsets | ForEach-Object { [version]$_.Name } | Sort-Object -Descending | Select-Object -First 1)
	Write-Host "MSVC $version is already installed in '$installationPath'."
	Write-Host 'Select toolset 14.53 or newer when configuring; tools\build.ps1 uses 14.53 by default.'
	return
}

$installer = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\setup.exe'
if (-not (Test-Path -LiteralPath $installer)) {
	throw "Visual Studio Installer was not found: $installer. Repair or install the Visual Studio Installer, then run this script again."
}

Write-Host "Adding the latest MSVC x86/x64 toolset to the Visual Studio installation at '$installationPath'."
& $installer modify --installPath $installationPath --add $component --passive --norestart
$installerExitCode = $LASTEXITCODE
if ($installerExitCode -notin @(0, 3010)) {
	throw "Visual Studio Installer failed with exit code $installerExitCode."
}

$installedToolsets = @()
if (Test-Path -LiteralPath $toolsetsPath) {
	$installedToolsets = @(Get-ChildItem -LiteralPath $toolsetsPath -Directory | Where-Object {
		$version = $null
		[version]::TryParse($_.Name, [ref]$version) -and $version -ge $minimumToolset
	})
}
if (-not $installedToolsets) {
	throw "Visual Studio Installer completed (exit code $installerExitCode), but MSVC $minimumToolset or newer was not found in '$toolsetsPath'. Update this Visual Studio installation or choose a channel that provides the required toolset."
}

$version = ($installedToolsets | ForEach-Object { [version]$_.Name } | Sort-Object -Descending | Select-Object -First 1)
Write-Host "MSVC $version is installed. Configure the project with toolset 14.53 or newer."
if ($installerExitCode -eq 3010) {
	Write-Host 'Visual Studio Installer reports that a restart is required.'
}
