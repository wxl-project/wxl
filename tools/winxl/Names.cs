using System.ComponentModel;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.Win32;

namespace Wxl.Winxl;

/// <summary>
/// The names winxl gives a solution and its projects. A name is the name of a directory and of the
/// files of a project, the namespace of a library and the base name of its header, so it is a C++
/// identifier that none of these readers takes for something else.
/// </summary>
internal static partial class Names
{
    [GeneratedRegex("^[A-Za-z][A-Za-z0-9_]*$")]
    private static partial Regex Identifier();

    /// <summary>The keywords and alternative tokens of C++23, which cannot name a namespace.</summary>
    private static readonly HashSet<string> Keywords = new(StringComparer.Ordinal)
    {
        "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch",
        "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const", "consteval", "constexpr",
        "constinit", "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype", "default", "delete",
        "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float", "for",
        "friend", "goto", "if", "inline", "int", "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq",
        "nullptr", "operator", "or", "or_eq", "private", "protected", "public", "register", "reinterpret_cast",
        "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct",
        "switch", "template", "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename",
        "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while",
    };

    /// <summary>
    /// The namespaces a source of wxl sees: a library of this name would be ambiguous with them
    /// in a source that opens wxl (<c>using namespace wxl;</c> brings wxl::core, wxl::dsl and wxl::impl).
    /// </summary>
    private static readonly HashSet<string> Namespaces = new(StringComparer.OrdinalIgnoreCase)
    {
        "std", "wxl", "core", "dsl", "impl", "detail", "fmt", "winrt", "Windows", "Microsoft",
    };

    /// <summary>The device names of Windows, which no file or directory can have, whatever the extension.</summary>
    [GeneratedRegex("^(CON|PRN|AUX|NUL|COM[0-9]|LPT[0-9])$", RegexOptions.IgnoreCase)]
    private static partial Regex DeviceName();

    /// <summary>
    /// Macros the headers of Windows and of the CRT define with names a project could take: the
    /// preprocessor would replace the namespace of a library of that name before the compiler sees
    /// it (<c>namespace small</c> is <c>namespace char</c> after rpcndr.h). Compared with case, as the
    /// preprocessor compares. A list kept by hand: the headers define thousands of macros, most of
    /// them names no one gives a library, and the functions of Windows that have a variant for each
    /// character set (CreateWindow is CreateWindowW) are among them.
    /// </summary>
    private static readonly HashSet<string> Macros = new(StringComparer.Ordinal)
    {
        // windef.h, minwindef.h, winnt.h, rpcndr.h, objbase.h, combaseapi.h, wingdi.h, winuser.h.
        "interface", "small", "hyper", "far", "near", "FAR", "NEAR", "pascal", "PASCAL", "cdecl", "CDECL",
        "WINAPI", "APIENTRY", "CALLBACK", "IN", "OUT", "OPTIONAL", "CONST", "VOID", "TRUE", "FALSE", "DELETE",
        "ERROR", "min", "max", "TRANSPARENT", "OPAQUE", "ABSOLUTE", "RELATIVE", "PURE", "THIS", "STDMETHOD",
        "MAX_PATH", "INFINITE", "SYNCHRONIZE", "NO_ERROR", "S_OK", "S_FALSE", "E_FAIL",
        "GetCurrentTime", "GetMessage", "SendMessage", "PostMessage", "CreateWindow", "LoadString", "GetObject",
        "DrawText", "MessageBox", "CreateFile", "DeleteFile", "CopyFile", "MoveFile", "GetClassName",
        "RegisterClass", "FindWindow", "Yield",
        // The CRT.
        "errno", "stdin", "stdout", "stderr", "assert", "offsetof", "NULL", "EOF", "BUFSIZ", "FILENAME_MAX",
        "RAND_MAX", "EXIT_SUCCESS", "EXIT_FAILURE", "CHAR_BIT", "INT_MAX", "UINT_MAX", "SIZE_MAX", "setjmp",
        "va_start", "va_arg", "va_end", "va_copy", "alloca",
    };

    public static void Check(string name, string what)
    {
        string? why = null;
        if (!Identifier().IsMatch(name))
        {
            why = "a letter, then letters, digits and _";
        }
        else if (name.Contains("__", StringComparison.Ordinal))
        {
            why = "a name with __ is reserved in C++";
        }
        else if (Keywords.Contains(name))
        {
            why = "it is a keyword of C++";
        }
        else if (Namespaces.Contains(name))
        {
            why = "it is a namespace a source on wxl sees";
        }
        else if (DeviceName().IsMatch(name))
        {
            why = "it is the name of a device of Windows, which no directory can have";
        }
        else if (Macros.Contains(name))
        {
            why = "it is a macro of the headers of Windows or of the CRT, which would replace the name in the sources";
        }
        if (why is not null)
        {
            throw new ToolException(Failure.Invocation, $"{what} '{name}' is not a name winxl gives a project: {why}");
        }
    }

    /// <summary>
    /// Stops on a library whose header &lt;name&gt;.h has the name of a header a project of the
    /// solution includes by name. Every project that references the library gets its directory as an
    /// include directory, which the compiler searches before the include path of the toolset: a
    /// library named math or windows would stand in for math.h or windows.h, a library named ui for
    /// ui.h of the profile. The headers are those of the directories that are searched by name:
    /// include\wxl and include\wxl\ui of the package and the directory of each of its components; the
    /// ucrt, um, shared, winrt and cppwinrt directories of every Windows SDK installed; and the include
    /// directory of every MSVC toolset of every Visual Studio installation vswhere finds. A header is
    /// the file &lt;name&gt;.h itself, compared without case as Windows compares names; a header without
    /// an extension (string, memory) is never &lt;name&gt;.h.
    /// </summary>
    public static void CheckHeader(string name, string what)
    {
        var directories = PackageDirectories().Concat(WindowsSdkDirectories()).Concat(MsvcDirectories());
        var file = $"{name}.h";
        foreach (var directory in directories)
        {
            var header = Path.Combine(directory, file);
            if (File.Exists(header))
            {
                throw new ToolException(Failure.Invocation,
                    $"{what} '{name}' is not a name winxl gives a library: its header {file} would stand in for {header}, which sources include by name");
            }
        }
    }

    /// <summary>The directories of the package winxl is part of: winxl lies in build\native\bin.</summary>
    private static List<string> PackageDirectories()
    {
        var package = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, ".."));
        var directories = new List<string> { Path.Combine(package, "include", "wxl"), Path.Combine(package, "include", "wxl", "ui") };
        var components = Path.Combine(package, "share", "wxl", "components");
        foreach (var directory in directories.Append(components))
        {
            if (!Directory.Exists(directory))
            {
                throw new ToolException(Failure.Environment,
                    $"no {directory}: winxl checks the name of a library against the headers of the package WinXL it is part of, "
                    + "and runs from build\\native\\bin of that package; the winxl of a build of the tree has no package beside it");
            }
        }
        directories.AddRange(Directory.GetDirectories(components));
        return directories;
    }

    /// <summary>
    /// The include directories of every Windows SDK installed, under the root the SDK installer
    /// records in the registry (KitsRoot10 of Installed Roots), in the view of either bitness.
    /// </summary>
    private static List<string> WindowsSdkDirectories()
    {
        string? root = null;
        foreach (var view in new[] { RegistryView.Registry64, RegistryView.Registry32 })
        {
            using var machine = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, view);
            using var roots = machine.OpenSubKey(@"SOFTWARE\Microsoft\Windows Kits\Installed Roots");
            root = roots?.GetValue("KitsRoot10") as string;
            if (root is not null)
            {
                break;
            }
        }
        var include = root is null ? null : Path.Combine(root, "Include");
        if (include is null || !Directory.Exists(include))
        {
            throw new ToolException(Failure.Environment,
                @"no Windows SDK is installed (no KitsRoot10 in HKLM\SOFTWARE\Microsoft\Windows Kits\Installed Roots): winxl checks "
                + "the name of a library against the headers of the Windows SDK, which an application on wxl is built with");
        }
        var directories = new List<string>();
        foreach (var version in Directory.GetDirectories(include))
        {
            foreach (var part in new[] { "ucrt", "um", "shared", "winrt", "cppwinrt" })
            {
                var directory = Path.Combine(version, part);
                if (Directory.Exists(directory))
                {
                    directories.Add(directory);
                }
            }
        }
        return directories;
    }

    /// <summary>
    /// The include directory of every MSVC toolset of every installation of Visual Studio or of its
    /// Build Tools with the C++ toolset, as vswhere of the Visual Studio Installer lists them.
    /// </summary>
    private static List<string> MsvcDirectories()
    {
        var vswhere = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86),
            "Microsoft Visual Studio", "Installer", "vswhere.exe");
        if (!File.Exists(vswhere))
        {
            throw new ToolException(Failure.Environment,
                $"no {vswhere}: winxl checks the name of a library against the headers of the MSVC toolsets, which it finds with "
                + "vswhere of the Visual Studio Installer; install Visual Studio or its Build Tools with the C++ toolset");
        }
        var start = new ProcessStartInfo(vswhere)
        {
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            StandardOutputEncoding = Encoding.UTF8,
        };
        foreach (var argument in new[] { "-products", "*", "-prerelease", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                                         "-property", "installationPath", "-utf8" })
        {
            start.ArgumentList.Add(argument);
        }
        List<string> installations;
        try
        {
            using var process = Process.Start(start)
                ?? throw new ToolException(Failure.Environment, $"{vswhere} did not start");
            var error = process.StandardError.ReadToEndAsync();
            installations = process.StandardOutput.ReadToEnd()
                .Split('\n', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries).ToList();
            process.WaitForExit();
            if (process.ExitCode != 0)
            {
                throw new ToolException(Failure.Environment, $"{vswhere} exited with {process.ExitCode}: {error.Result.Trim()}");
            }
        }
        catch (Win32Exception e)
        {
            throw new ToolException(Failure.Environment, $"{vswhere} cannot be run: {e.Message}", e);
        }
        var directories = installations
            .Select(installation => Path.Combine(installation, "VC", "Tools", "MSVC"))
            .Where(Directory.Exists)
            .SelectMany(Directory.GetDirectories)
            .Select(toolset => Path.Combine(toolset, "include"))
            .Where(Directory.Exists)
            .ToList();
        if (directories.Count == 0)
        {
            throw new ToolException(Failure.Environment,
                "vswhere finds no installation of Visual Studio or of its Build Tools with an MSVC toolset: winxl checks the name of "
                + "a library against the headers of the toolsets, which an application on wxl is built with");
        }
        return directories;
    }
}
