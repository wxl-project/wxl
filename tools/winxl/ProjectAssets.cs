using System.Text.Json;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace Wxl.Winxl;

/// <summary>A package the restore of a project resolved.</summary>
/// <param name="Version">Its version, normalized.</param>
/// <param name="Directory">The directory of the package in the package folder of the restore.</param>
/// <param name="AssetsPath">The project.assets.json that names it.</param>
internal sealed record ResolvedPackage(string Version, string Directory, string AssetsPath);

/// <summary>
/// What the restore of NuGet resolved for a project, from the project.assets.json it writes into
/// $(RestoreOutputPath): by default $(MSBuildProjectExtensionsPath), which is by default
/// $(BaseIntermediateOutputPath), obj\ beside the project.
/// </summary>
internal static partial class ProjectAssets
{
    private static readonly string[] Locations = ["BaseIntermediateOutputPath", "MSBuildProjectExtensionsPath", "RestoreOutputPath"];

    /// <summary>
    /// The path of the project.assets.json of <paramref name="projectPath"/>, there or not. The
    /// properties that move it are taken from where a project of C++ gets them: the nearest
    /// Directory.Build.props above the project, the project itself and the nearest
    /// Directory.Build.targets, in the order MSBuild evaluates them, with the defaults of
    /// Microsoft.Common.props at the import of Microsoft.Cpp.Default.props. A value or a condition
    /// beyond what is evaluated here stops the command: a guess would compare with a restore that
    /// is not the project's.
    /// </summary>
    public static string FilePath(string projectPath)
    {
        var directory = Path.GetDirectoryName(projectPath)!;
        var values = Locations.ToDictionary(name => name, _ => "", StringComparer.OrdinalIgnoreCase);
        var defaulted = false;
        void Defaults()
        {
            var intermediate = WithSlash(values["BaseIntermediateOutputPath"] is { Length: > 0 } set ? set : @"obj\");
            values["BaseIntermediateOutputPath"] = intermediate;
            values["MSBuildProjectExtensionsPath"] = WithSlash(Path.Combine(directory,
                values["MSBuildProjectExtensionsPath"] is { Length: > 0 } extensions ? extensions : intermediate));
            defaulted = true;
        }

        if (FileAbove(directory, "Directory.Build.props") is { } props)
        {
            Assign(values, props, projectPath, Solution.LoadXml(props).Root!.Elements(), null);
        }
        Assign(values, projectPath, projectPath, Solution.LoadXml(projectPath).Root!.Elements(), Defaults);
        if (!defaulted)
        {
            Defaults();
        }
        if (FileAbove(directory, "Directory.Build.targets") is { } targets)
        {
            Assign(values, targets, projectPath, Solution.LoadXml(targets).Root!.Elements(), null);
        }
        var output = values["RestoreOutputPath"] is { Length: > 0 } restore ? restore : values["MSBuildProjectExtensionsPath"];
        return Path.GetFullPath(Path.Combine(directory, output, "project.assets.json"));
    }

    /// <summary>
    /// The package <paramref name="id"/> as the last restore of the project at
    /// <paramref name="projectPath"/> resolved it, or null when there is no restore of the project as
    /// it is now: no project.assets.json, one of another project file, one from before the
    /// reference became <paramref name="reference"/>, or one that resolved no <paramref name="id"/>.
    /// A file that is there and cannot be read, or holds versions of the package that differ
    /// between its targets, stops the command.
    /// </summary>
    public static ResolvedPackage? Resolved(string projectPath, string id, string reference)
    {
        var path = FilePath(projectPath);
        if (!File.Exists(path))
        {
            return null;
        }
        JsonDocument document;
        try
        {
            document = JsonDocument.Parse(File.ReadAllBytes(path));
        }
        catch (JsonException e)
        {
            throw new ToolException(Failure.Input, $"{path}, the restore of {projectPath}, is not JSON: {e.Message}", e);
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            throw new ToolException(Failure.Environment, $"{path}, the restore of {projectPath}, cannot be read: {e.Message}", e);
        }

        using (document)
        {
            var root = document.RootElement;
            var project = Member(root, "project", JsonValueKind.Object, path);
            var restored = Member(Member(project, "restore", JsonValueKind.Object, path), "projectPath", JsonValueKind.String, path).GetString()!;
            if (!string.Equals(Path.GetFullPath(restored), projectPath, StringComparison.OrdinalIgnoreCase))
            {
                return null;
            }

            // The range each framework requested, which NuGet writes from the reference it restored.
            var requested = Member(project, "frameworks", JsonValueKind.Object, path).EnumerateObject()
                .Select(framework => framework.Value.TryGetProperty("dependencies", out var dependencies) && dependencies.ValueKind == JsonValueKind.Object
                    ? dependencies.EnumerateObject().FirstOrDefault(d => string.Equals(d.Name, id, StringComparison.OrdinalIgnoreCase)).Value
                    : default)
                .Where(dependency => dependency.ValueKind == JsonValueKind.Object)
                .Select(dependency => dependency.TryGetProperty("version", out var range) && range.ValueKind == JsonValueKind.String ? range.GetString()! : "")
                .ToList();
            if (requested.Count == 0 || requested.Any(range => !NuGetVersion.RangeMatches(reference, range)))
            {
                return null;
            }

            // Each target names the package it resolved as <id>/<version>.
            var resolved = new List<(string Target, string Key, string Version)>();
            foreach (var target in Member(root, "targets", JsonValueKind.Object, path).EnumerateObject())
            {
                foreach (var library in target.Value.EnumerateObject())
                {
                    var slash = library.Name.IndexOf('/');
                    if (slash > 0 && string.Equals(library.Name[..slash], id, StringComparison.OrdinalIgnoreCase)
                        && library.Value.TryGetProperty("type", out var type) && type.ValueEquals("package"))
                    {
                        var version = NuGetVersion.Normalize(library.Name[(slash + 1)..])
                            ?? throw new ToolException(Failure.Input, $"{path}: the target {target.Name} names {library.Name}, whose version is not one of NuGet");
                        resolved.Add((target.Name, library.Name, version));
                    }
                }
            }
            if (resolved.Count == 0)
            {
                return null;
            }
            if (resolved.Select(r => r.Version).Distinct(StringComparer.OrdinalIgnoreCase).Count() > 1)
            {
                throw new ToolException(Failure.Input,
                    $"the restore of {projectPath} resolved different versions of {id} for its targets ({path}): "
                    + string.Join(", ", resolved.Select(r => $"{r.Version} for {r.Target}")));
            }

            var key = resolved[0].Key;
            var libraries = Member(root, "libraries", JsonValueKind.Object, path);
            var packagePath = Member(Member(libraries, key, JsonValueKind.Object, path), "path", JsonValueKind.String, path).GetString()!;
            var folders = Member(root, "packageFolders", JsonValueKind.Object, path).EnumerateObject()
                .Select(folder => Path.Combine(folder.Name, packagePath)).ToList();
            var packageDirectory = folders.FirstOrDefault(Directory.Exists) ?? folders.FirstOrDefault()
                ?? throw new ToolException(Failure.Input, $"{path} names no package folder");
            return new ResolvedPackage(resolved[0].Version, Path.GetFullPath(packageDirectory), path);
        }
    }

    /// <summary>
    /// The assignments of <paramref name="elements"/> to the properties that move project.assets.json;
    /// <paramref name="defaults"/> runs at the import of Microsoft.Cpp.Default.props.
    /// </summary>
    private static void Assign(Dictionary<string, string> values, string file, string projectPath, IEnumerable<XElement> elements, Action? defaults)
    {
        foreach (var element in elements)
        {
            if (element.Name.LocalName == "Import")
            {
                if (defaults is not null
                    && (element.Attribute("Project")?.Value ?? "").EndsWith("Microsoft.Cpp.Default.props", StringComparison.OrdinalIgnoreCase))
                {
                    defaults();
                    defaults = null;
                }
                continue;
            }
            if (element.Name.LocalName != "PropertyGroup")
            {
                continue;
            }
            foreach (var property in element.Elements().Where(p => Locations.Contains(p.Name.LocalName, StringComparer.OrdinalIgnoreCase)))
            {
                var name = property.Name.LocalName;
                if (element.Attribute("Condition") is not null || !Unset(property.Attribute("Condition")?.Value, name))
                {
                    throw new ToolException(Failure.Input,
                        $"{file} sets {name} under a condition, and winxl evaluates only the condition '$({name})' == '' "
                        + $"of {name} itself to find project.assets.json, the restore of {projectPath}");
                }
                if (property.Attribute("Condition") is not null && values[name].Length > 0)
                {
                    continue;
                }
                values[name] = Expand(property.Value.Trim(), name, file, projectPath, values);
            }
        }
    }

    /// <summary>Whether a condition is absent or the guard '$(name)' == '' of the property it is on.</summary>
    private static bool Unset(string? condition, string name) =>
        condition is null || (Guard().Match(condition) is { Success: true } match
            && string.Equals(match.Groups[1].Value, name, StringComparison.OrdinalIgnoreCase));

    /// <summary>
    /// <paramref name="value"/> with the properties of a path in it replaced: the directory of the
    /// file, the directory, name and file of the project and the properties of the location.
    /// </summary>
    private static string Expand(string value, string name, string file, string projectPath, Dictionary<string, string> values)
    {
        var expanded = Property().Replace(value, match => match.Groups[1].Value.ToLowerInvariant() switch
        {
            "msbuildthisfiledirectory" => WithSlash(Path.GetDirectoryName(file)!),
            "msbuildprojectdirectory" => Path.GetDirectoryName(projectPath)!,
            "msbuildprojectname" => Path.GetFileNameWithoutExtension(projectPath),
            "msbuildprojectfile" => Path.GetFileName(projectPath),
            _ when values.TryGetValue(match.Groups[1].Value, out var location) => location,
            _ => match.Value,
        });
        if (expanded.Contains("$(") || expanded.Contains("@(") || expanded.Contains("%("))
        {
            throw new ToolException(Failure.Input,
                $"{file} sets {name} to {value}, and winxl evaluates there only $(MSBuildThisFileDirectory), $(MSBuildProjectDirectory), "
                + $"$(MSBuildProjectName), $(MSBuildProjectFile) and {string.Join(", ", Locations.Select(l => $"$({l})"))} "
                + $"to find project.assets.json, the restore of {projectPath}");
        }
        return expanded;
    }

    private static string? FileAbove(string directory, string name)
    {
        for (var current = directory; current is not null; current = Path.GetDirectoryName(current))
        {
            var candidate = Path.Combine(current, name);
            if (File.Exists(candidate))
            {
                return candidate;
            }
        }
        return null;
    }

    private static string WithSlash(string path) => path.EndsWith('\\') || path.EndsWith('/') ? path : path + '\\';

    private static JsonElement Member(JsonElement parent, string name, JsonValueKind kind, string path)
    {
        if (parent.ValueKind != JsonValueKind.Object || !parent.TryGetProperty(name, out var value) || value.ValueKind != kind)
        {
            throw new ToolException(Failure.Input,
                $"{path} is not a restore of NuGet: it has no {(kind == JsonValueKind.String ? "string" : "object")} \"{name}\" where the restore writes one");
        }
        return value;
    }

    [GeneratedRegex(@"\$\(([A-Za-z_][A-Za-z0-9_.-]*)\)")]
    private static partial Regex Property();

    [GeneratedRegex(@"^\s*'\$\(([A-Za-z_][A-Za-z0-9_.-]*)\)'\s*==\s*''\s*$")]
    private static partial Regex Guard();
}
