# Writes the names of the macros defined in a source of a solution on wxl, which winxl embeds and
# refuses as the name of a library (tools/winxl/Names.cs): the preprocessor would replace the
# namespace of a library of that name. cmake/wxl_winxl.cmake runs it when winxl is built, with the
# compiler of the build in its developer environment, whose INCLUDE names the headers.
#
# Usage: powershell.exe -NoProfile -File Write-HeaderMacros.ps1 -Compiler <cl.exe> -Probe <source>
#            -Settings <file>[|<file>...] -Out <file> -Depfile <file>
#
#   -Probe     the system headers a source includes, cmake/header-macros.cpp
#   -Settings  one file per configuration of the tree, one setting per line: "option <option>",
#              "define <definition>", "include <directory>" or "header <public header of wxl>"
#   -Out       the names, one per line, each ended by LF, sorted by ordinal comparison
#   -Depfile   the headers the compiler read, as the dependencies of -Out
#
# A translation unit includes the probe and then the public headers, and is preprocessed with the
# options, the definitions and the include directories of the settings; the names it could define
# are those of the #defines of the headers the compiler reports it read (/sourceDependencies) and of
# the definitions. A second translation unit includes the first and tests each of these names with
# #ifdef, writing the name as a string literal, which the preprocessor does not replace, when it is
# defined: the names it writes are the macros defined at the end of the first. Both steps are the
# documented /EP /P. The names of every configuration are written, and a configuration whose
# settings are those of another is not preprocessed again.
#
# Names that begin with _ or hold __ are left out: Names.Check refuses them before it looks at
# macros (a name begins with a letter, and __ is reserved in C++), so the list would carry them for
# nothing.
#
# Compatible with Windows PowerShell 5.1: no `?.`, `??`, ternary, `&&`.

param(
    [Parameter(Mandatory=$true)] [string]$Compiler,
    [Parameter(Mandatory=$true)] [string]$Probe,
    [Parameter(Mandatory=$true)] [string]$Settings,
    [Parameter(Mandatory=$true)] [string]$Out,
    [Parameter(Mandatory=$true)] [string]$Depfile
)

$ErrorActionPreference = 'Stop'

$identifier = '[A-Za-z][A-Za-z0-9_]*'
$define = [regex]::new("^[ \t]*#[ \t]*define[ \t]+($identifier)",
    [System.Text.RegularExpressions.RegexOptions]'Multiline, Compiled, CultureInvariant')
# The line before the tests: the names come after it.
$marker = '"winxl: the macros defined at the end of the probe"'

# An argument of a response file of cl, quoted as the C runtime reads a command line.
function ConvertTo-Argument([string]$Text) {
    if ($Text -notmatch '[\s"]') { return $Text }
    $quoted = [regex]::Replace($Text, '(\\*)"', { param($m) $m.Groups[1].Value + $m.Groups[1].Value + '\"' })
    $quoted = [regex]::Replace($quoted, '(\\+)$', '$1$1')
    return '"' + $quoted + '"'
}

# Runs the preprocessor with <Arguments> through the response file <Base>.rsp: stops with what cl wrote
# when it fails.
function Invoke-Preprocessor([string]$Base, [string[]]$Arguments) {
    $response = "$Base.rsp"
    # One line: cl takes the argument of a switch such as /sourceDependencies only from the same line.
    $line = (@('/nologo', '/EP', '/P') + $Arguments | ForEach-Object { ConvertTo-Argument $_ }) -join ' '
    [System.IO.File]::WriteAllText($response, $line, [System.Text.Encoding]::Unicode)
    $ErrorActionPreference = 'Continue'
    $output = & $Compiler "@$response" 2>&1 | ForEach-Object { "$_" }
    $code = $LASTEXITCODE
    $ErrorActionPreference = 'Stop'
    if ($code -ne 0) {
        throw "$Compiler @$response exited with ${code}:`n$($output -join "`n")"
    }
}

$base = [System.IO.Path]::Combine([System.IO.Path]::GetDirectoryName($Out), [System.IO.Path]::GetFileNameWithoutExtension($Out))
$probePath = [System.IO.Path]::GetFullPath($Probe).Replace('\', '/')
$names = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$headers = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
# The names a configuration could define: those of another configuration are tested too, and are not
# read again.
$candidates = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$seen = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach ($file in $Settings.Split('|')) {
    $text = [System.IO.File]::ReadAllText($file)
    if (-not $seen.Add($text)) { continue }
    $options = [System.Collections.Generic.List[string]]::new()
    $source = [System.Text.StringBuilder]::new()
    [void]$source.Append("#include `"$probePath`"`n")
    foreach ($line in $text.Split("`n")) {
        $line = $line.TrimEnd("`r")
        if ($line -eq '') { continue }
        $at = $line.IndexOf(' ')
        if ($at -lt 0) { throw "${file}: '$line' is no setting" }
        $value = $line.Substring($at + 1)
        switch ($line.Substring(0, $at)) {
            'option' { $options.Add($value) }
            'define' {
                $options.Add("/D$value")
                if ($value -match "^($identifier)" -and -not $Matches[1].Contains('__')) { [void]$candidates.Add($Matches[1]) }
            }
            'include' { $options.Add("/I$value") }
            'header' { [void]$source.Append("#include `"$($value.Replace('\', '/'))`"`n") }
            default { throw "${file}: '$line' is no setting" }
        }
    }

    $first = "$base.probe.cpp"
    [System.IO.File]::WriteAllText($first, $source.ToString())
    Invoke-Preprocessor "$base.probe" ($options.ToArray() + @("/Fi$base.probe.i", '/sourceDependencies', "$base.probe.json", $first))
    [System.IO.File]::Delete("$base.probe.i")
    $read = (Get-Content -LiteralPath "$base.probe.json" -Raw | ConvertFrom-Json).Data.Includes
    foreach ($header in $read) {
        if (-not $headers.Add($header)) { continue }
        foreach ($match in $define.Matches([System.IO.File]::ReadAllText($header))) {
            $name = $match.Groups[1].Value
            if (-not $name.Contains('__')) { [void]$candidates.Add($name) }
        }
    }

    $check = [System.Text.StringBuilder]::new()
    [void]$check.Append("#include `"$($first.Replace('\', '/'))`"`n$marker`n")
    foreach ($name in $candidates) { [void]$check.Append("#ifdef $name`n`"$name`"`n#endif`n") }
    $second = "$base.check.cpp"
    [System.IO.File]::WriteAllText($second, $check.ToString())
    Invoke-Preprocessor "$base.check" ($options.ToArray() + @("/Fi$base.check.i", $second))
    $output = [System.IO.File]::ReadAllText("$base.check.i")
    [System.IO.File]::Delete("$base.check.i")
    $after = $output.LastIndexOf($marker)
    if ($after -lt 0) { throw "$base.check.i has no line $marker before the names" }
    foreach ($match in [regex]::Matches($output.Substring($after + $marker.Length), "(?m)^`"($identifier)`"\r?$")) {
        [void]$names.Add($match.Groups[1].Value)
    }
}
if ($names.Count -eq 0) {
    throw "The probe defines no macro: the headers ${Probe} includes were not read."
}

$sorted = [string[]]::new($names.Count)
$names.CopyTo($sorted)
[Array]::Sort($sorted, [StringComparer]::Ordinal)
$builder = [System.Text.StringBuilder]::new()
foreach ($name in $sorted) { [void]$builder.Append($name).Append("`n") }
[System.IO.File]::WriteAllText($Out, $builder.ToString(), [System.Text.UTF8Encoding]::new($false))

# The rule of a Makefile, which ninja reads: a space, # and $ escaped.
function ConvertTo-DependencyPath([string]$Path) {
    return $Path.Replace('\', '/').Replace(' ', '\ ').Replace('#', '\#').Replace('$', '$$')
}
$rule = [System.Text.StringBuilder]::new()
[void]$rule.Append((ConvertTo-DependencyPath ([System.IO.Path]::GetFullPath($Out)))).Append(':')
foreach ($header in $headers) { [void]$rule.Append(" \`n  ").Append((ConvertTo-DependencyPath $header)) }
[System.IO.File]::WriteAllText($Depfile, $rule.Append("`n").ToString(), [System.Text.UTF8Encoding]::new($false))
