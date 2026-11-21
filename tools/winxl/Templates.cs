using System.Reflection;
using System.Text;

namespace Wxl.Winxl;

/// <summary>
/// The files <c>winxl new</c> and <c>winxl add</c> write, from the templates in Templates/ beside
/// this file, which winxl.csproj embeds. A template names what differs from one file to the next
/// as @NAME@ and the like; every token is replaced, and a token left over is a fault of the tool.
/// </summary>
internal static class Templates
{
    /// <summary>The text of the template <paramref name="name"/> with the tokens replaced.</summary>
    public static string Expand(string name, IReadOnlyDictionary<string, string> tokens)
    {
        var assembly = typeof(Templates).Assembly;
        using var stream = assembly.GetManifestResourceStream($"Templates/{name}")
            ?? throw new InvalidOperationException($"winxl was built without the template {name}");
        using var reader = new StreamReader(stream, Encoding.UTF8);
        var text = new StringBuilder(reader.ReadToEnd());
        foreach (var (token, value) in tokens)
        {
            text.Replace($"@{token}@", value);
        }
        var expanded = text.ToString();
        var left = expanded.IndexOf('@');
        while (left >= 0)
        {
            var right = expanded.IndexOf('@', left + 1);
            if (right < 0)
            {
                break;
            }
            var inner = expanded.AsSpan(left + 1, right - left - 1);
            if (inner.Length > 0 && IsToken(inner))
            {
                throw new InvalidOperationException($"the template {name} has the token @{inner}@, which winxl does not fill");
            }
            left = right;
        }
        return expanded;
    }

    private static bool IsToken(ReadOnlySpan<char> text)
    {
        foreach (var c in text)
        {
            if (!(c is >= 'A' and <= 'Z' or '_'))
            {
                return false;
            }
        }
        return true;
    }
}
