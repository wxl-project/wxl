using System.Xml;
using System.Xml.Linq;

namespace Wxl.Winxl;

/// <summary>A project of a solution, as winxl reads it from its .vcxproj.</summary>
/// <param name="Name">The name of the project: the base name of its file.</param>
/// <param name="Path">The path of the .vcxproj, absolute.</param>
/// <param name="RelativePath">The path of the .vcxproj as the solution names it, with /.</param>
/// <param name="Guid">The ProjectGuid of the project, with its braces.</param>
/// <param name="Type">The ConfigurationType: Application or StaticLibrary.</param>
/// <param name="Profile">The WinXLProfile of the project: set in the profile project only.</param>
/// <param name="References">The projects it references, each the absolute path of its file.</param>
/// <param name="ReferencesWinXL">Whether it references the package WinXL (a PackageReference).</param>
internal sealed record SolutionProject(
    string Name, string Path, string RelativePath, string Guid, string Type, string? Profile,
    IReadOnlyList<string> References, bool ReferencesWinXL)
{
    public bool Uses(SolutionProject other) =>
        References.Any(r => string.Equals(r, other.Path, StringComparison.OrdinalIgnoreCase));
}

/// <summary>
/// A solution of kind wxl, which <c>winxl new wxl</c> writes and <c>winxl add</c> extends: an XML
/// solution file (.slnx) of Visual Studio and MSBuild, beside it a directory per project with its
/// .vcxproj. One project names the profile of the solution (WinXLProfile) and builds the library of
/// that profile; one is the program, which references every other. winxl reads the C++ projects
/// (.vcxproj) of the solution and leaves any other project as it is.
/// </summary>
internal sealed class Solution
{
    public const string Kind = "wxl";
    public const string ProfileProjectName = "wxl";
    private const string MsBuild = "http://schemas.microsoft.com/developer/msbuild/2003";
    private static readonly XNamespace Ns = MsBuild;

    public string Path { get; }
    public string Directory { get; }
    public IReadOnlyList<SolutionProject> Projects { get; }

    private Solution(string path, IReadOnlyList<SolutionProject> projects)
    {
        Path = path;
        Directory = System.IO.Path.GetDirectoryName(path)!;
        Projects = projects;
    }

    /// <summary>
    /// The solution at <paramref name="path"/>: a .slnx file, or a directory with exactly one.
    /// </summary>
    public static Solution Open(string path)
    {
        var file = path;
        if (System.IO.Directory.Exists(path))
        {
            var found = System.IO.Directory.GetFiles(path, "*.slnx");
            file = found.Length == 1
                ? found[0]
                : throw new ToolException(Failure.Input,
                    found.Length == 0
                        ? $"no solution (.slnx) in {System.IO.Path.GetFullPath(path)}: run winxl in the directory of the solution winxl new wrote, or name it with --solution"
                        : $"{System.IO.Path.GetFullPath(path)} holds more than one solution (.slnx): name one with --solution");
        }
        file = System.IO.Path.GetFullPath(file);
        if (!File.Exists(file))
        {
            throw new ToolException(Failure.Input, $"no solution {file}");
        }

        var document = LoadXml(file);
        var directory = System.IO.Path.GetDirectoryName(file)!;
        var projects = new List<SolutionProject>();
        foreach (var element in document.Root!.Descendants("Project"))
        {
            var relative = element.Attribute("Path")?.Value
                ?? throw new ToolException(Failure.Input, $"{file}: a Project without a Path");
            if (string.Equals(System.IO.Path.GetExtension(relative), ".vcxproj", StringComparison.OrdinalIgnoreCase))
            {
                projects.Add(ReadProject(directory, relative));
            }
        }
        return new Solution(file, projects);
    }

    /// <summary>The project that builds the library of the profile.</summary>
    public SolutionProject ProfileProject => Single(p => p.Profile is not null, "names a profile (WinXLProfile)");

    /// <summary>The program of the solution.</summary>
    public SolutionProject Program => Single(p => p.Type == "Application", "is a program (ConfigurationType Application)");

    private SolutionProject Single(Func<SolutionProject, bool> match, string what)
    {
        var found = Projects.Where(match).ToList();
        return found.Count == 1
            ? found[0]
            : throw new ToolException(Failure.Input,
                $"{Path}: {(found.Count == 0 ? "no C++ project" : $"{found.Count} C++ projects ({string.Join(", ", found.Select(p => p.Name))})")} "
                + $"{what}, and a solution of kind {Kind} has one");
    }

    /// <summary>The solution file with the project at <paramref name="relativePath"/> added, not yet written.</summary>
    public XDocument WithProject(string relativePath)
    {
        var document = LoadXml(Path, preserveWhitespace: true);
        var root = document.Root!;
        var last = root.Elements("Project").LastOrDefault();
        var element = new XElement("Project", new XAttribute("Path", relativePath));
        if (last is null)
        {
            root.Add(new XText("  "), element, new XText("\n"));
        }
        else
        {
            last.AddAfterSelf(new XText("\n  "), element);
        }
        return document;
    }

    /// <summary>Writes the solution file of a new solution with <paramref name="projects"/>.</summary>
    public static void Write(string path, IEnumerable<string> projects)
    {
        var document = new XDocument(new XElement("Solution",
            new XText("\n  "),
            new XElement("Configurations", new XText("\n    "), new XElement("Platform", new XAttribute("Name", "x64")), new XText("\n  "))));
        foreach (var project in projects)
        {
            document.Root!.Add(new XText("\n  "), new XElement("Project", new XAttribute("Path", project)));
        }
        document.Root!.Add(new XText("\n"));
        FileSystem.Save(document, path);
    }

    /// <summary>
    /// The project file of <paramref name="project"/> with a ProjectReference to <paramref name="target"/>
    /// added, not yet written: in the group of its references, or in a group of its own before the
    /// PackageReference to WinXL.
    /// </summary>
    public static XDocument WithReference(SolutionProject project, SolutionProject target)
    {
        var document = LoadXml(project.Path, preserveWhitespace: true);
        var root = document.Root!;
        var include = System.IO.Path.GetRelativePath(System.IO.Path.GetDirectoryName(project.Path)!, target.Path);
        var references = root.Elements(Ns + "ItemGroup").Elements(Ns + "ProjectReference").ToList();
        var reference = ProjectReference(include, target.Guid, "    ");
        if (references.Count > 0)
        {
            references[^1].AddAfterSelf(new XText("\n    "), reference);
        }
        else
        {
            var group = new XElement(Ns + "ItemGroup", new XText("\n    "), reference, new XText("\n  "));
            var package = root.Elements(Ns + "ItemGroup").LastOrDefault(g => g.Elements(Ns + "PackageReference").Any())
                ?? throw new ToolException(Failure.Input, $"{project.Path} has no PackageReference to WinXL");
            package.AddBeforeSelf(group, new XText("\n\n  "));
        }
        return document;
    }

    /// <summary>The ItemGroup of the references of a new project, or nothing, as text of its template.</summary>
    public static string ReferenceGroup(string projectDirectory, IEnumerable<SolutionProject> targets)
    {
        var list = targets.ToList();
        if (list.Count == 0)
        {
            return "";
        }
        // A path comes from the solution file, where it may hold any character: the XML API writes
        // it escaped. The element is made without a namespace, which the root of the template declares.
        XNamespace none = "";
        var group = new XElement(none + "ItemGroup");
        foreach (var target in list)
        {
            group.Add(new XText("\n    "),
                new XElement(none + "ProjectReference",
                    new XAttribute("Include", System.IO.Path.GetRelativePath(projectDirectory, target.Path)),
                    new XText("\n      "), new XElement(none + "Project", target.Guid), new XText("\n    ")));
        }
        group.Add(new XText("\n  "));
        return $"\n  {group.ToString(SaveOptions.DisableFormatting)}\n";
    }

    private static XElement ProjectReference(string include, string guid, string indent) =>
        new(Ns + "ProjectReference", new XAttribute("Include", include),
            new XText($"\n{indent}  "), new XElement(Ns + "Project", guid), new XText($"\n{indent}"));

    private static SolutionProject ReadProject(string directory, string relative)
    {
        var path = System.IO.Path.GetFullPath(System.IO.Path.Combine(directory, relative));
        if (!File.Exists(path))
        {
            throw new ToolException(Failure.Input, $"the solution names the project {relative}, and there is no {path}");
        }
        var document = LoadXml(path);
        string? Property(string name) => document.Root!.Descendants(Ns + name).Select(e => e.Value.Trim()).FirstOrDefault();
        var guid = Property("ProjectGuid") ?? throw new ToolException(Failure.Input, $"{path} has no ProjectGuid");
        var type = Property("ConfigurationType") ?? throw new ToolException(Failure.Input, $"{path} has no ConfigurationType");
        var projectDirectory = System.IO.Path.GetDirectoryName(path)!;
        var references = document.Root!.Descendants(Ns + "ProjectReference")
            .Select(r => r.Attribute("Include")?.Value)
            .OfType<string>()
            .Select(include => System.IO.Path.GetFullPath(System.IO.Path.Combine(projectDirectory, include)))
            .ToList();
        var winxl = document.Root!.Descendants(Ns + "PackageReference")
            .Any(r => string.Equals(r.Attribute("Include")?.Value, "WinXL", StringComparison.OrdinalIgnoreCase));
        return new SolutionProject(System.IO.Path.GetFileNameWithoutExtension(path), path, relative.Replace('\\', '/'),
            guid, type, Property("WinXLProfile"), references, winxl);
    }

    private static XDocument LoadXml(string path, bool preserveWhitespace = false)
    {
        try
        {
            return XDocument.Load(path, preserveWhitespace ? LoadOptions.PreserveWhitespace : LoadOptions.None);
        }
        catch (XmlException e)
        {
            throw new ToolException(Failure.Input, $"{path} is not XML: {e.Message}", e);
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            throw new ToolException(Failure.Environment, $"{path} cannot be read: {e.Message}", e);
        }
    }
}
