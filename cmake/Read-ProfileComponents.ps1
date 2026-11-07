# Reads a profile chain and its component manifests, printing one line per item
# CMake needs: chain component names, and for each selected component after needs
# closure, its directory, sources and libraries.
#
# Usage: Read-ProfileComponents.ps1 -Profile <path> [-ComponentDir <dir>] ...
#        powershell.exe -NoProfile -File Read-ProfileComponents.ps1 -Profile ...
#
# Compatible with Windows PowerShell 5.1: no `?.`, `??`, ternary, `&&`.
# Output format: <type>:<value>, one per line. CMake parses it with regex.
#   chain:<name>       - a component from the profile's own list (before needs)
#   component:<name>   - a selected component (after needs closure)
#   dir:<path>         - the component directory for the preceding component
#   source:<path>      - a source file (relative to component dir)
#   library:<target>   - a CMake library target

param(
    [Parameter(Mandatory=$true)]
    [string]$Profile,
    [string[]]$ComponentDir = @()
)

$ErrorActionPreference = 'Stop'

function Read-JsonWithComments([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "Profile not found: $Path"
    }
    $text = [IO.File]::ReadAllText($Path, [Text.Encoding]::UTF8)
    # Strip // comments and /* */ comments outside strings
    $inString = $false
    $escaped = $false
    $result = New-Object Text.StringBuilder
    for ($i = 0; $i -lt $text.Length; $i++) {
        $ch = $text[$i]
        if ($escaped) {
            $null = $result.Append($ch)
            $escaped = $false
            continue
        }
        if ($inString) {
            $null = $result.Append($ch)
            if ($ch -eq '\') { $escaped = $true }
            elseif ($ch -eq '"') { $inString = $false }
            continue
        }
        if ($ch -eq '"') {
            $null = $result.Append($ch)
            $inString = $true
            continue
        }
        if ($ch -eq '/' -and $i+1 -lt $text.Length) {
            $next = $text[$i+1]
            if ($next -eq '/') {
                # Skip to end of line
                while ($i -lt $text.Length -and $text[$i] -ne "`n") { $i++ }
                if ($i -lt $text.Length) { $null = $result.Append($text[$i]) }
                continue
            }
            if ($next -eq '*') {
                # Skip to */
                $i += 2
                while ($i+1 -lt $text.Length -and -not ($text[$i] -eq '*' -and $text[$i+1] -eq '/')) { $i++ }
                $i++
                continue
            }
        }
        $null = $result.Append($ch)
    }
    return ConvertFrom-Json -InputObject $result.ToString()
}

function Resolve-ProfileChain([string]$Path) {
    $prof = Read-JsonWithComments $Path
    $chain = @()
    if ($prof.extends) {
        $dir = Split-Path $Path -Parent
        foreach ($parent in $prof.extends) {
            $parentPath = if ($parent -like '*.winxl-profile') {
                Join-Path $dir $parent
            } else {
                Join-Path $dir ($parent + '.winxl-profile')
            }
            $chain += Resolve-ProfileChain $parentPath
        }
    }
    return $chain + @($prof)
}

function Find-ComponentManifest([string]$Name, [string[]]$Dirs) {
    $found = @()
    foreach ($dir in $Dirs) {
        $manifest = Join-Path $dir ($Name + '/component.json')
        if (Test-Path -LiteralPath $manifest -PathType Leaf) {
            $found += $manifest
        }
    }
    if ($found.Count -eq 0) {
        throw "Component not found: $Name (searched $($Dirs.Count) directories)"
    }
    if ($found.Count -gt 1) {
        throw "Component $Name found in multiple directories: $($found -join ', ')"
    }
    return $found[0]
}

$chain = Resolve-ProfileChain $Profile
$chainComponents = @()
foreach ($prof in $chain) {
    if ($prof.components) {
        foreach ($comp in $prof.components) {
            if ($chainComponents -notcontains $comp) {
                $chainComponents += $comp
            }
        }
    }
}

# Print chain components
foreach ($name in $chainComponents) {
    Write-Output "chain:$name"
}

if ($ComponentDir.Count -eq 0) {
    exit 0
}

# Load and close components by needs
$loaded = @{}
$selected = @()
function Import-Component([string]$Name) {
    if ($loaded.ContainsKey($Name)) { return }
    $manifest = Find-ComponentManifest $Name $ComponentDir
    $comp = Read-JsonWithComments $manifest
    $loaded[$Name] = $comp
    if ($comp.needs) {
        foreach ($dep in $comp.needs) {
            Import-Component $dep
        }
    }
    $script:selected += $Name
}

foreach ($name in $chainComponents) {
    Import-Component $name
}

# Print selected components with their details
foreach ($name in $selected) {
    $comp = $loaded[$name]
    $manifest = Find-ComponentManifest $name $ComponentDir
    $dir = (Split-Path $manifest -Parent).Replace('\','/')
    Write-Output "component:$name"
    Write-Output "dir:$dir"
    if ($comp.sources) {
        foreach ($src in $comp.sources) {
            Write-Output "source:$src"
        }
    }
    if ($comp.libraries) {
        foreach ($lib in $comp.libraries) {
            Write-Output "library:$lib"
        }
    }
}
