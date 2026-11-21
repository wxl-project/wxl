using System.Text.RegularExpressions;

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
        if (why is not null)
        {
            throw new ToolException(Failure.Invocation, $"{what} '{name}' is not a name winxl gives a project: {why}");
        }
    }

    /// <summary>
    /// Stops on a library whose header &lt;name&gt;.h would have the name of a header of the package:
    /// a source that includes the header of wxl by name would find the library's, or the reverse --
    /// in its own directory the header of the library finds itself first. The headers are those of the
    /// directories a project on the package includes by name: include\wxl and include\wxl\ui of the
    /// package, and the directory of every component, which a library on the profile gets.
    /// </summary>
    public static void CheckHeader(string name, string what)
    {
        var package = PackageRoot();
        var directories = new List<string> { Path.Combine(package, "include", "wxl"), Path.Combine(package, "include", "wxl", "ui") };
        var components = Path.Combine(package, "share", "wxl", "components");
        foreach (var directory in directories.Append(components))
        {
            if (!Directory.Exists(directory))
            {
                throw new ToolException(Failure.Environment,
                    $"no {directory}: winxl checks the name of a library against the headers of the package WinXL it is part of, "
                    + "and runs from build\\native\\bin of that package");
            }
        }
        directories.AddRange(Directory.GetDirectories(components));
        foreach (var directory in directories)
        {
            foreach (var header in Directory.EnumerateFiles(directory))
            {
                if (Path.GetExtension(header).ToLowerInvariant() is ".h" or ".hpp" or ".hxx"
                    && string.Equals(Path.GetFileNameWithoutExtension(header), name, StringComparison.OrdinalIgnoreCase))
                {
                    throw new ToolException(Failure.Invocation,
                        $"{what} '{name}' is not a name winxl gives a library: its header {name}.h would have the name of {header}, which sources include by name");
                }
            }
        }
    }

    /// <summary>build\native of the package: winxl lies in its bin.</summary>
    private static string PackageRoot() => Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, ".."));
}
