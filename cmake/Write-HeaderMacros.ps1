# Writes the names of the macros the headers of the Windows SDK and of the MSVC toolset define,
# which winxl embeds and refuses as the name of a library (tools/winxl/Names.cs): the
# preprocessor would replace the namespace of a library of that name. cmake/wxl_winxl.cmake runs
# it when winxl is built, on the headers of the toolset and the SDK of the build.
#
# Usage: powershell.exe -NoProfile -File Write-HeaderMacros.ps1 -Headers <list> -Out <file>
#
#   -Headers  a file of the headers, one path per line
#   -Out      the names, one per line, each ended by LF, sorted by ordinal comparison
#
# A name is that of every line #define <name>, object-like or function-like, whatever #if it is
# under: the configuration of the consumer may differ from any one reading. Names that begin with
# _ or hold __ are left out: Names.Check refuses them before it looks at macros (a name begins
# with a letter, and __ is reserved in C++), so the list would carry them for nothing.
#
# Compatible with Windows PowerShell 5.1: no `?.`, `??`, ternary, `&&`.

param(
    [Parameter(Mandatory=$true)] [string]$Headers,
    [Parameter(Mandatory=$true)] [string]$Out
)

$ErrorActionPreference = 'Stop'

$define = [regex]::new('^[ \t]*#[ \t]*define[ \t]+([A-Za-z][A-Za-z0-9_]*)',
    [System.Text.RegularExpressions.RegexOptions]'Multiline, Compiled, CultureInvariant')
$names = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach ($header in [System.IO.File]::ReadAllLines($Headers)) {
    if ($header -eq '') { continue }
    # ReadAllText follows a byte order mark; a header without one is read as UTF-8, and the
    # bytes a name is made of are ASCII in every encoding the headers use.
    $text = [System.IO.File]::ReadAllText($header)
    foreach ($match in $define.Matches($text)) {
        $name = $match.Groups[1].Value
        if (-not $name.Contains('__')) { [void]$names.Add($name) }
    }
}
if ($names.Count -eq 0) {
    throw "No #define in the headers of ${Headers}: the list of the headers is empty or wrong."
}

$sorted = [string[]]::new($names.Count)
$names.CopyTo($sorted)
[Array]::Sort($sorted, [StringComparer]::Ordinal)
$builder = [System.Text.StringBuilder]::new()
foreach ($name in $sorted) { [void]$builder.Append($name).Append("`n") }
[System.IO.File]::WriteAllText($Out, $builder.ToString(), [System.Text.UTF8Encoding]::new($false))
