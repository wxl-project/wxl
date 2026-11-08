# Reads a profile chain and its component manifests, printing one line per item
# CMake needs: chain component names, and for each selected component after needs
# closure, its directory, sources, schema and surface fragments and libraries.
#
# Usage: Read-ProfileComponents.ps1 -Profile <path> [-ComponentDir <dir>] ...
#        powershell.exe -NoProfile -File Read-ProfileComponents.ps1 -Profile ...
#
# Compatible with Windows PowerShell 5.1: no `?.`, `??`, ternary, `&&`.
# Output format: <type>:<value>, one per line. CMake parses it with regex.
#   profile:<path>     - a profile of the chain, the given one last
#   chain:<name>       - a component from the profile's own list (before needs)
#   component:<name>   - a selected component (after needs closure)
#   dir:<path>         - the component directory for the preceding component
#   source:<path>      - a source file (relative to component dir)
#   schema:<path>      - the component's schema fragment (relative to component dir)
#   surface:<path>     - the component's surface test fragment (relative to component dir)
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

# Where a profile named in "extends" lives, as the generator resolves it: a reference
# with a directory part or an extension is a path relative to the referring profile's
# directory; a bare name is <name>.winxl-profile beside it.
function Resolve-Reference([string]$Reference, [string]$Referrer) {
    $dir = Split-Path $Referrer -Parent
    if ([IO.Path]::IsPathRooted($Reference)) { return $Reference }
    if ($Reference.Contains('/') -or $Reference.Contains('\') -or [IO.Path]::HasExtension($Reference)) {
        return Join-Path $dir $Reference
    }
    return Join-Path $dir ($Reference + '.winxl-profile')
}

# The chain, each profile after those it extends, loaded once; a profile that extends
# itself through others stops the script.
$script:profilesLoaded = @{}
$script:profilesLoading = @{}
function Resolve-ProfileChain([string]$Path) {
    $key = [IO.Path]::GetFullPath($Path).ToLowerInvariant()
    if ($script:profilesLoaded.ContainsKey($key)) { return @() }
    if ($script:profilesLoading.ContainsKey($key)) { throw "Profile reference cycle at $Path" }
    $script:profilesLoading[$key] = $true
    $prof = Read-JsonWithComments $Path
    $chain = @()
    if ($prof.extends) {
        foreach ($parent in $prof.extends) {
            $chain += Resolve-ProfileChain (Resolve-Reference $parent $Path)
        }
    }
    $script:profilesLoading.Remove($key)
    $script:profilesLoaded[$key] = $true
    return $chain + @(@{ path = [IO.Path]::GetFullPath($Path); profile = $prof })
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
foreach ($entry in $chain) {
    Write-Output ("profile:" + $entry.path.Replace('\','/'))
    $prof = $entry.profile
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

# Load and close components by needs, each after those it needs; components that need
# each other in a cycle stop the script.
$loaded = @{}
$loading = @{}
$selected = @()
function Import-Component([string]$Name) {
    if ($loaded.ContainsKey($Name)) { return }
    if ($loading.ContainsKey($Name)) { throw "Components need each other in a cycle at $Name" }
    $loading[$Name] = $true
    $manifest = Find-ComponentManifest $Name $ComponentDir
    $comp = Read-JsonWithComments $manifest
    if ($comp.needs) {
        foreach ($dep in $comp.needs) {
            Import-Component $dep
        }
    }
    $loading.Remove($Name)
    $loaded[$Name] = $comp
    $script:selected += $Name
}

foreach ($name in $chainComponents) {
    Import-Component $name
}

# Print selected components with their details
foreach ($name in $selected) {
    $comp = $loaded[$name]
    $manifest = Find-ComponentManifest $name $ComponentDir
    # One spelling per directory: CMake declares the manifest beneath it as a dependency.
    $dir = [IO.Path]::GetFullPath((Split-Path $manifest -Parent)).Replace('\','/')
    Write-Output "component:$name"
    Write-Output "dir:$dir"
    if ($comp.sources) {
        foreach ($src in $comp.sources) {
            Write-Output "source:$src"
        }
    }
    if ($comp.schema) {
        Write-Output "schema:$($comp.schema)"
    }
    if ($comp.surface) {
        Write-Output "surface:$($comp.surface)"
    }
    if ($comp.libraries) {
        foreach ($lib in $comp.libraries) {
            Write-Output "library:$lib"
        }
    }
}
