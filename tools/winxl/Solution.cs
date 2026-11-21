using System.Text.RegularExpressions;
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
internal sealed record SolutionProject(string Name, string Path, string RelativePath, string Guid, string Type, string? Profile);

/// <summary>
/// A solution of kind wxl, which <c>winxl new wxl</c> writes and <c>winxl add</c> extends: an XML
/// solution file (.slnx) of Visual Studio and MSBuild, beside it a directory per project with its
/// .vcxproj. One project names the profile of the solution (WinXLProfile) and builds the library of
/// that profile; one is the program, which references every other.
/// </summary>
internal sealed partial class Solution
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
    /// A name of a project or of a solution: a C++ identifier, since it names a namespace and the
    /// files of the project, and nothing a path or MSBuild would read as syntax.
    /// </summary>
    public static void CheckName(string name, string what)
    {
        if (!NamePattern().IsMatch(name))
        {
            throw new ToolException(Failure.Invocation,
                $"{what} '{name}' is not a name winxl gives a project: a letter, then letters, digits and _");
        }
    }

    [GeneratedRegex("^[A-Za-z][A-Za-z0-9_]*$")]
    private static partial Regex NamePattern();

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
            projects.Add(ReadProject(directory, relative));
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
                $"{Path}: {(found.Count == 0 ? "no project" : $"{found.Count} projects ({string.Join(", ", found.Select(p => p.Name))})")} "
                + $"{what}, and a solution of kind {Kind} has one");
    }

    /// <summary>Adds the project at <paramref name="relativePath"/> to the solution file.</summary>
    public void Add(string relativePath)
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
        SaveXml(document, Path);
    }

    /// <summary>Writes the solution file of a new solution with <paramref name="projects"/>.</summary>
    public static void Write(string path, IEnumerable<string> projects)
    {
        var lines = new List<string>
        {
            "<Solution>",
            "  <Configurations>",
            "    <Platform Name=\"x64\" />",
            "  </Configurations>",
        };
        lines.AddRange(projects.Select(p => $"  <Project Path=\"{p}\" />"));
        lines.Add("</Solution>");
        Templates.Write(path, string.Join("\n", lines) + "\n");
    }

    /// <summary>Adds to the project <paramref name="project"/> a ProjectReference to <paramref name="target"/>.</summary>
    public static void Reference(SolutionProject project, SolutionProject target)
    {
        var document = LoadXml(project.Path, preserveWhitespace: true);
        var root = document.Root!;
        var include = System.IO.Path.GetRelativePath(System.IO.Path.GetDirectoryName(project.Path)!, target.Path);
        var references = root.Elements(Ns + "ItemGroup").Elements(Ns + "ProjectReference").ToList();
        if (references.Any(r => string.Equals(r.Attribute("Include")?.Value, include, StringComparison.OrdinalIgnoreCase)))
        {
            return;
        }
        var reference = ProjectReference(include, target.Guid, "    ");
        if (references.Count > 0)
        {
            references[^1].AddAfterSelf(new XText("\n    "), reference);
        }
        else
        {
            // A group of its own before the PackageReference, the last group of a project winxl writes.
            var group = new XElement(Ns + "ItemGroup", new XText("\n    "), reference, new XText("\n  "));
            var package = root.Elements(Ns + "ItemGroup").LastOrDefault(g => g.Elements(Ns + "PackageReference").Any())
                ?? throw new ToolException(Failure.Input, $"{project.Path} has no PackageReference to WinXL");
            package.AddBeforeSelf(group, new XText("\n\n  "));
        }
        SaveXml(document, project.Path);
    }

    /// <summary>The ItemGroup of the references of a new project, or nothing.</summary>
    public static string ReferenceGroup(string projectDirectory, IEnumerable<SolutionProject> targets)
    {
        var list = targets.ToList();
        if (list.Count == 0)
        {
            return "";
        }
        // The names of the projects are identifiers (CheckName), so a path needs no escaping.
        var lines = new List<string> { "", "  <ItemGroup>" };
        foreach (var target in list)
        {
            lines.Add($"    <ProjectReference Include=\"{System.IO.Path.GetRelativePath(projectDirectory, target.Path)}\">");
            lines.Add($"      <Project>{target.Guid}</Project>");
            lines.Add("    </ProjectReference>");
        }
        lines.Add("  </ItemGroup>");
        return string.Join("\n", lines) + "\n";
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
        return new SolutionProject(System.IO.Path.GetFileNameWithoutExtension(path), path, relative.Replace('\\', '/'), guid, type, Property("WinXLProfile"));
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

    private static void SaveXml(XDocument document, string path)
    {
        var settings = new XmlWriterSettings
        {
            Encoding = new System.Text.UTF8Encoding(encoderShouldEmitUTF8Identifier: false),
            NewLineChars = "\r\n",
            NewLineHandling = NewLineHandling.Replace,
            OmitXmlDeclaration = document.Declaration is null,
        };
        using var writer = XmlWriter.Create(path, settings);
        document.Save(writer);
    }
}
