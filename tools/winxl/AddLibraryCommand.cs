namespace Wxl.Winxl;

/// <summary>
/// <c>winxl add library --name &lt;name&gt; [--ui] [--references &lt;library&gt;,...] [--solution &lt;solution&gt;] [--json]</c>:
/// adds a static library to a solution of kind wxl, in the directory &lt;name&gt; beside the solution
/// file, with &lt;name&gt;.h and &lt;name&gt;.cpp to start from; the program of the solution references it.
/// </summary>
/// <remarks>
/// Without --ui the library stands on wxl.core alone: the package WinXL gives it the modules of
/// wxl. With --ui it references the profile project as well and gets the interface of the library
/// of the profile -- the wrappers, the components, wxl.ui.common -- to build elements of the user
/// interface with. --references names libraries of the solution this one uses: it references
/// them, and gets their headers, which every library gives as its public include directory. The
/// solution is &lt;solution&gt;, a .slnx or the directory of one, or the one .slnx in the current
/// directory.
/// </remarks>
internal static class AddLibraryCommand
{
    public static int Run(string name, bool ui, string? references, string? solutionPath, bool json)
    {
        Solution.CheckName(name, "--name");
        var solution = Solution.Open(solutionPath ?? Directory.GetCurrentDirectory());
        if (solution.Projects.Any(p => string.Equals(p.Name, name, StringComparison.OrdinalIgnoreCase)))
        {
            throw new ToolException(Failure.Input, $"{solution.Path} has a project {name} already");
        }
        var profile = solution.ProfileProject;
        var program = solution.Program;

        // The libraries of the application this one stands on: their headers and archives.
        var libraries = new List<SolutionProject>();
        foreach (var reference in (references ?? "").Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
        {
            var used = solution.Projects.FirstOrDefault(p => string.Equals(p.Name, reference, StringComparison.OrdinalIgnoreCase))
                ?? throw new ToolException(Failure.Input, $"--references names {reference}, and {solution.Path} has no project {reference}");
            if (used == profile || used == program || used.Type != "StaticLibrary")
            {
                throw new ToolException(Failure.Invocation,
                    $"--references names {reference}, which is not a library of the application: the profile project is referenced with --ui");
            }
            libraries.Add(used);
        }
        if (references is not null && libraries.Count == 0)
        {
            throw new ToolException(Failure.Invocation, "--references needs the names of libraries of the solution, separated by commas");
        }

        var directory = Path.Combine(solution.Directory, name);
        if (File.Exists(directory) || Directory.Exists(directory))
        {
            throw new ToolException(Failure.Input, $"{directory} is there already, and winxl add writes the library into a new directory");
        }
        Directory.CreateDirectory(directory);

        var version = VersionCommand.TreeVersion();
        var guid = NewCommand.NewGuid();
        var tokens = new Dictionary<string, string> { ["NAME"] = name };
        var variant = ui ? "ui" : "core";
        Templates.Write(Path.Combine(directory, $"{name}.h"), Templates.Expand($"library-{variant}.h", tokens));
        Templates.Write(Path.Combine(directory, $"{name}.cpp"), Templates.Expand($"library-{variant}.cpp", tokens));

        var project = Path.Combine(directory, $"{name}.vcxproj");
        Templates.Write(project, Templates.Expand("project.vcxproj", new Dictionary<string, string>
        {
            ["COMMENT"] = ui
                ? $"""
                    {name}, a static library of the application on the profile of the solution: it references the
                    profile project, ..\{Solution.ProfileProjectName}, whose library gives it the wrappers, the components and
                    wxl.ui.common. The program links the library of the profile.
                  """
                : $"""
                    {name}, a static library of the application on wxl.core: the package WinXL gives it the modules of
                    wxl, and the program links their archives.
                  """,
            ["GUID"] = guid[1..^1],
            ["NAME"] = name,
            ["TYPE"] = "StaticLibrary",
            ["PROPERTIES"] =
                "    <!-- The headers of the library, which MSBuild gives the projects that reference it. -->\n"
                + "    <PublicIncludeDirectories>$(MSBuildProjectDirectory)</PublicIncludeDirectories>\n",
            ["ITEMS"] = $"    <ClInclude Include=\"{name}.h\" />\n    <ClCompile Include=\"{name}.cpp\" />\n",
            ["REFERENCES"] = Solution.ReferenceGroup(directory, ui ? [profile, .. libraries] : libraries),
            ["VERSION"] = version,
        }));

        var relative = $"{name}/{name}.vcxproj";
        solution.Add(relative);
        var library = new SolutionProject(name, project, relative, guid, "StaticLibrary", null);
        Solution.Reference(program, library);

        Output.Log($"{project}: a static library on {(ui ? $"the profile {profile.Profile}" : "wxl.core")}, referenced by {program.Name}");
        if (json)
        {
            Output.Json(writer =>
            {
                writer.WriteString("solution", solution.Path);
                writer.WriteStartObject("project");
                writer.WriteString("name", name);
                writer.WriteString("path", project);
                writer.WriteString("kind", "library");
                writer.WriteBoolean("ui", ui);
                writer.WriteStartArray("references");
                foreach (var used in libraries)
                {
                    writer.WriteStringValue(used.Name);
                }
                writer.WriteEndArray();
                writer.WriteEndObject();
                writer.WriteStartArray("referencedBy");
                writer.WriteStringValue(program.Name);
                writer.WriteEndArray();
            });
        }
        else
        {
            Console.Out.WriteLine(project);
        }
        return 0;
    }
}
