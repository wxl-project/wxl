namespace Wxl.Winxl;

/// <summary>
/// NuGet versions and version ranges as far as winxl compares them: the normalized form of a
/// version, and whether the range a restore records for a reference is the one NuGet writes for it.
/// </summary>
internal static class NuGetVersion
{
    /// <summary>
    /// <paramref name="text"/> in NuGet's normalized form, or null when it is not a version: three
    /// numeric parts and a fourth only when it is not 0, each without leading zeros, then the label;
    /// the build metadata, which no comparison looks at, is dropped. "1.0" is "1.0.0", "0.1.0.900"
    /// stays as it is.
    /// </summary>
    public static string? Normalize(string text)
    {
        text = text.Trim();
        var plus = text.IndexOf('+');
        var version = plus < 0 ? text : text[..plus];
        var dash = version.IndexOf('-');
        var label = dash < 0 ? "" : version[(dash + 1)..];
        var numbers = (dash < 0 ? version : version[..dash]).Split('.');
        if (numbers.Length > 4 || (dash >= 0 && !Identifiers(label)) || (plus >= 0 && !Identifiers(text[(plus + 1)..])))
        {
            return null;
        }
        var parts = new int[4];
        for (var i = 0; i < numbers.Length; i++)
        {
            if (numbers[i].Length == 0 || !numbers[i].All(char.IsAsciiDigit) || !int.TryParse(numbers[i], out parts[i]))
            {
                return null;
            }
        }
        return $"{parts[0]}.{parts[1]}.{parts[2]}{(parts[3] != 0 ? $".{parts[3]}" : "")}{(dash < 0 ? "" : $"-{label}")}";
    }

    /// <summary>Whether two versions are the same version: their labels compare without case, as NuGet's do.</summary>
    public static bool Same(string a, string b) =>
        Normalize(a) is { } first && Normalize(b) is { } second && string.Equals(first, second, StringComparison.OrdinalIgnoreCase);

    /// <summary>
    /// Whether <paramref name="requested"/>, the range project.assets.json records for a reference,
    /// is the one NuGet writes for <paramref name="reference"/>, the Version of the PackageReference:
    /// a version V as [V, ), a floating version V* as [V*, ), [V] as [V, V], a range as itself, the
    /// versions compared normalized.
    /// </summary>
    public static bool RangeMatches(string reference, string requested) =>
        Range(reference) is { } written && Range(requested) is { } recorded && written == recorded;

    /// <summary>A range with its bounds normalized, an empty bound for an open side; null when it is not a range.</summary>
    private static (char Open, string Lower, string Upper, char Close)? Range(string text)
    {
        text = text.Trim();
        if (text.Length == 0)
        {
            return null;
        }
        if (text[0] is not ('[' or '('))
        {
            return Bound(text) is { Length: > 0 } minimum ? ('[', minimum, "", ')') : null;
        }
        if (text[^1] is not (']' or ')'))
        {
            return null;
        }
        var bounds = text[1..^1].Split(',');
        if (bounds.Length == 1)
        {
            return text[0] == '[' && text[^1] == ']' && Bound(bounds[0]) is { Length: > 0 } held ? ('[', held, held, ']') : null;
        }
        if (bounds.Length != 2 || Bound(bounds[0]) is not { } lower || Bound(bounds[1]) is not { } upper)
        {
            return null;
        }
        // A side without a bound is open whatever bracket it is written with.
        return (lower.Length == 0 ? '(' : text[0], lower, upper, upper.Length == 0 ? ')' : text[^1]);
    }

    /// <summary>A bound of a range: empty, a floating version as written, or a normalized version; null otherwise.</summary>
    private static string? Bound(string text)
    {
        text = text.Trim();
        return text.Length == 0 ? "" : text.Contains('*') ? text.ToLowerInvariant() : Normalize(text)?.ToLowerInvariant();
    }

    /// <summary>Dot-separated identifiers of [0-9A-Za-z-], none empty: a label or build metadata.</summary>
    private static bool Identifiers(string text) =>
        text.Split('.').All(identifier => identifier.Length > 0 && identifier.All(c => char.IsAsciiLetterOrDigit(c) || c == '-'));
}
