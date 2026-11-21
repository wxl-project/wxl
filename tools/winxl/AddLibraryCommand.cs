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
/// them, and gets their headers, which every library gives as its public include directory. A
/// library it uses that stands on the profile brings the profile project along: the headers of that
/// library include those of the profile, and MSBuild gives the include directories of a project only
/// to the projects that reference it directly. The solution is &lt;solution&gt;, a .slnx or the
/// directory of one, or the one .slnx in the current directory.
///
/// Everything is checked before anything is written, so that a refused command leaves the
/// solution as it was.
/// </remarks>
internal static class AddLibraryCommand
{
    public static int Run(string name, bool ui, string? references, string? solutionPath, bool json)
    {
        Names.Check(name, "--name");
        var solution = Solution.Open(solutionPath ?? Directory.GetCurrentDirectory());
        if (solution.Projects.Any(p => string.Equals(p.Name, name, StringComparison.OrdinalIgnoreCase)))
        {
            throw new ToolException(Failure.Input, $"{solution.Path} has a project {name} already");
        }
        var profile = solution.ProfileProject;
        var program = solution.Program;
        // The new library references WinXL at the version of the solution, and that is the package
        // whose headers its name is checked against and whose templates winxl writes: the winxl of
        // another version writes for another package.
        var version = program.WinXLVersion
            ?? throw new ToolException(Failure.Input, $"the program {program.Path} has no PackageReference to WinXL");
        var own = VersionCommand.TreeVersion();
        if (!string.Equals(version, own, StringComparison.OrdinalIgnoreCase))
        {
            var packages = Environment.GetEnvironmentVariable("NUGET_PACKAGES") is { Length: > 0 } folder
                ? folder
                : Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".nuget", "packages");
            throw new ToolException(Failure.Input,
                $"the solution references WinXL {version}, and this winxl is of WinXL {own}: run the winxl of the package "
                + $"WinXL {version}, which the restore of the solution puts at {Path.Combine(packages, "winxl", version.ToLowerInvariant(), "build", "native", "bin", "winxl.exe")}");
        }
        Names.CheckHeader(name, "--name");

        // The libraries of the application this one stands on: their headers and archives.
        var libraries = new List<SolutionProject>();
        foreach (var reference in (references ?? "").Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
        {
            var used = solution.Projects.FirstOrDefault(p => string.Equals(p.Name, reference, StringComparison.OrdinalIgnoreCase))
                ?? throw new ToolException(Failure.Input, $"--references names {reference}, and {solution.Path} has no C++ project {reference}");
            if (used == profile || used == program || used.Type != "StaticLibrary")
            {
                throw new ToolException(Failure.Invocation,
                    $"--references names {reference}, which is not a library of the application: the profile project is referenced with --ui");
            }
            if (!libraries.Contains(used))
            {
                libraries.Add(used);
            }
        }
        if (references is not null && libraries.Count == 0)
        {
            throw new ToolException(Failure.Invocation, "--references needs the names of libraries of the solution, separated by commas");
        }
        var throughLibraries = libraries.Where(l => l.Uses(profile)).Select(l => l.Name).ToList();
        var onProfile = ui || throughLibraries.Count > 0;

        var directory = Path.Combine(solution.Directory, name);
        if (File.Exists(directory) || Directory.Exists(directory))
        {
            throw new ToolException(Failure.Input, $"{directory} is there already, and winxl add writes the library into a new directory");
        }

        var guid = NewCommand.NewGuid();
        var tokens = new Dictionary<string, string> { ["NAME"] = name };
        var variant = ui ? "ui" : "core";
        var header = Templates.Expand($"library-{variant}.h", tokens);
        var source = Templates.Expand($"library-{variant}.cpp", tokens);
        var project = Path.Combine(directory, $"{name}.vcxproj");
        var projectText = Templates.Expand("project.vcxproj", new Dictionary<string, string>
        {
            ["COMMENT"] = onProfile
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
            ["REFERENCES"] = Solution.ReferenceGroup(directory, onProfile ? [profile, .. libraries] : libraries),
            ["VERSION"] = version,
        });

        var relative = $"{name}/{name}.vcxproj";
        var library = new SolutionProject(name, project, relative, guid, "StaticLibrary", null, [], version);
        var solutionFile = solution.WithProject(relative);
        var programFile = Solution.WithReference(program, library);

        // A write the machine refuses undoes the writes before it: the solution file and the program
        // get their bytes back, and the directory of the library goes.
        var solutionBytes = FileSystem.Read(solution.Path);
        var programBytes = FileSystem.Read(program.Path);
        try
        {
            FileSystem.CreateDirectory(directory);
            FileSystem.Write(Path.Combine(directory, $"{name}.h"), header);
            FileSystem.Write(Path.Combine(directory, $"{name}.cpp"), source);
            FileSystem.Write(project, projectText);
            FileSystem.Save(solutionFile, solution.Path, FileFormat.Of(solutionBytes));
            FileSystem.Save(programFile, program.Path, FileFormat.Of(programBytes));
        }
        catch (ToolException failure)
        {
            throw FileSystem.Undone(failure,
                [FileSystem.Restore(solution.Path, solutionBytes), FileSystem.Restore(program.Path, programBytes), FileSystem.Remove(directory)]);
        }

        if (throughLibraries.Count > 0 && !ui)
        {
            Output.Log($"{name} references the profile project too, since it uses {string.Join(", ", throughLibraries)} on the profile, "
                + "whose headers include those of the profile");
        }
        Output.Log($"{project}: a static library on {(onProfile ? $"the profile {profile.Profile}" : "wxl.core")}, referenced by {program.Name}");
        if (json)
        {
            Output.Json(writer =>
            {
                writer.WriteString("solution", solution.Path);
                writer.WriteStartObject("project");
                writer.WriteString("name", name);
                writer.WriteString("path", project);
                writer.WriteString("kind", "library");
                writer.WriteBoolean("ui", onProfile);
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
