using System.Text.Json;

namespace Wxl.Winxl;

/// <summary>
/// A package of the restore: its id and version as the restore names them, and its directory,
/// absolute and canonical, with the separator of the platform.
/// </summary>
internal sealed record RestoredPackage(string Id, string Version, string Directory);

/// <summary>
/// The list of the restored packages a configure of the wxl tree writes,
/// <c>&lt;build&gt;/wxl.packages/restored-packages.json</c>; its format is described in
/// cmake/wxl_restored_packages.cmake:
/// <c>{ "packages": [ { "id", "version", "directory" }, ... ], "windowsSdk": { ... } }</c>.
/// </summary>
internal static class RestoredPackages
{
    /// <summary>The package <paramref name="id"/> of the list at <paramref name="listPath"/>: exactly one entry.</summary>
    public static RestoredPackage Find(string listPath, string id)
    {
        if (!File.Exists(listPath))
        {
            throw new ToolException(Failure.Input,
                $"no list of the restored packages at {listPath}: the configure of a build of the wxl "
                + "tree writes it, <build>/wxl.packages/restored-packages.json -- configure the tree first");
        }

        JsonDocument document;
        try
        {
            document = JsonDocument.Parse(File.ReadAllBytes(listPath));
        }
        catch (JsonException e)
        {
            throw new ToolException(Failure.Input, $"{listPath} is not JSON: {e.Message}", e);
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            throw new ToolException(Failure.Environment, $"{listPath} cannot be read: {e.Message}", e);
        }

        using (document)
        {
            if (document.RootElement.ValueKind != JsonValueKind.Object
                || !document.RootElement.TryGetProperty("packages", out var packages)
                || packages.ValueKind != JsonValueKind.Array)
            {
                throw new ToolException(Failure.Input, $"{listPath} has no array \"packages\"");
            }

            RestoredPackage? found = null;
            foreach (var entry in packages.EnumerateArray())
            {
                var entryId = Member(entry, "id", listPath);
                if (!string.Equals(entryId, id, StringComparison.OrdinalIgnoreCase))
                {
                    continue;
                }
                if (found is not null)
                {
                    throw new ToolException(Failure.Input, $"{listPath} lists {id} more than once");
                }
                found = new RestoredPackage(
                    entryId, Member(entry, "version", listPath), PackageDirectory(Member(entry, "directory", listPath), listPath));
            }

            return found ?? throw new ToolException(Failure.Input, $"the restore of {listPath} resolved no {id}");
        }
    }

    /// <summary>
    /// The directory of a package as the tool uses and prints it. The list writes it with /
    /// (cmake/wxl_restored_packages.cmake), and a path combined from it would keep that
    /// separator beside the platform's; GetFullPath turns it into the platform's and makes
    /// the path canonical, once, where it is read.
    /// </summary>
    private static string PackageDirectory(string directory, string listPath)
    {
        try
        {
            return Path.GetFullPath(directory);
        }
        catch (Exception e) when (e is ArgumentException or NotSupportedException or PathTooLongException)
        {
            throw new ToolException(Failure.Input, $"{listPath}: the directory '{directory}' is not a path: {e.Message}", e);
        }
    }

    private static string Member(JsonElement entry, string name, string listPath)
    {
        if (entry.ValueKind != JsonValueKind.Object
            || !entry.TryGetProperty(name, out var value)
            || value.ValueKind != JsonValueKind.String
            || string.IsNullOrEmpty(value.GetString()))
        {
            throw new ToolException(Failure.Input, $"{listPath}: an entry of \"packages\" has no string \"{name}\"");
        }
        return value.GetString()!;
    }
}
