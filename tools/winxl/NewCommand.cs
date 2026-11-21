namespace Wxl.Winxl;

/// <summary>
/// <c>winxl new wxl --name &lt;name&gt; [--dir &lt;directory&gt;] [--extends &lt;profile&gt;] [--json]</c>:
/// writes a solution of kind wxl -- an application on the WinUI layer of wxl, built by MSBuild from
/// the package WinXL of the version of this winxl.
/// </summary>
/// <remarks>
/// The solution &lt;name&gt;.slnx holds two projects, each in a directory of its own:
///
///   wxl      the profile project, a static library: the profile &lt;name&gt;.winxl-profile, which
///            extends the profile &lt;profile&gt; of the package (base by default), and WinXLProfile,
///            by which WinXL.targets of the package builds the library of that profile in it --
///            the wrappers and the components the profile selects -- and gives every project
///            that references it the interface of the library;
///   &lt;name&gt;   the program, which references the profile project, with wxl_launched opening a
///            window.
///
/// <c>winxl add library</c> adds the static libraries of the application.
/// </remarks>
internal static class NewCommand
{
    public const string DefaultExtends = "base";

    public static int Run(string kind, string name, string? directory, string? extends, bool json)
    {
        if (kind != Solution.Kind)
        {
            throw new ToolException(Failure.Invocation, $"'{kind}' is not a kind of solution winxl writes: winxl new {Solution.Kind} --name <name>");
        }
        Solution.CheckName(name, "--name");
        if (name == Solution.ProfileProjectName)
        {
            throw new ToolException(Failure.Invocation, $"--name {name} is the name of the profile project of the solution");
        }
        extends ??= DefaultExtends;
        Solution.CheckName(extends, "--extends");

        var root = Path.GetFullPath(directory ?? name);
        if (File.Exists(root) || (Directory.Exists(root) && Directory.EnumerateFileSystemEntries(root).Any()))
        {
            throw new ToolException(Failure.Input, $"{root} is there already: winxl new writes a solution into a new or empty directory");
        }

        var version = VersionCommand.TreeVersion();
        var profileGuid = NewGuid();
        var programGuid = NewGuid();
        var profileFile = $"{name}.winxl-profile";

        var profileDir = Directory.CreateDirectory(Path.Combine(root, Solution.ProfileProjectName)).FullName;
        var profileProject = Path.Combine(profileDir, $"{Solution.ProfileProjectName}.vcxproj");
        Templates.Write(Path.Combine(profileDir, profileFile),
            Templates.Expand("profile.winxl-profile", new Dictionary<string, string> { ["NAME"] = name, ["EXTENDS"] = extends }));
        Templates.Write(profileProject, Templates.Expand("project.vcxproj", new Dictionary<string, string>
        {
            ["COMMENT"] =
                $"""
                  The profile project of {name}: a static library that builds the library of the profile
                  {profileFile}, that is the wrappers, which the generator of the package WinXL writes from
                  the profile, and the components the profile selects. WinXL.targets of the package builds it by
                  the property WinXLProfile, and gives the projects that reference this one the interface of
                  the library: the include root of the wrappers, the directories of the components and the
                  headers of wxl.ui.common. A program that references it links the library.
                """,
            ["GUID"] = profileGuid[1..^1],
            ["NAME"] = Solution.ProfileProjectName,
            ["TYPE"] = "StaticLibrary",
            ["PROPERTIES"] = $"    <WinXLProfile>{profileFile}</WinXLProfile>\n",
            ["ITEMS"] = $"    <None Include=\"{profileFile}\" />\n",
            ["REFERENCES"] = "",
            ["VERSION"] = version,
        }));
        var profile = new SolutionProject(Solution.ProfileProjectName, profileProject,
            $"{Solution.ProfileProjectName}/{Solution.ProfileProjectName}.vcxproj", profileGuid, "StaticLibrary", profileFile);

        var programDir = Directory.CreateDirectory(Path.Combine(root, name)).FullName;
        var programProject = Path.Combine(programDir, $"{name}.vcxproj");
        Templates.Write(Path.Combine(programDir, "main.cpp"),
            Templates.Expand("program.cpp", new Dictionary<string, string> { ["NAME"] = name }));
        Templates.Write(programProject, Templates.Expand("project.vcxproj", new Dictionary<string, string>
        {
            ["COMMENT"] =
                $"""
                  The program of {name}. It references the profile project, ..\wxl, and every library of the
                  application: WinXL.targets of the package WinXL gives it the interface of the library of the
                  profile and links that library with wxl.ui.common, and copies the DLLs a program on wxl needs
                  beside it.
                """,
            ["GUID"] = programGuid[1..^1],
            ["NAME"] = name,
            ["TYPE"] = "Application",
            ["PROPERTIES"] = "",
            ["ITEMS"] = "    <ClCompile Include=\"main.cpp\" />\n",
            ["REFERENCES"] = Solution.ReferenceGroup(programDir, [profile]),
            ["VERSION"] = version,
        }));

        var solution = Path.Combine(root, $"{name}.slnx");
        Solution.Write(solution, [profile.RelativePath, $"{name}/{name}.vcxproj"]);

        Output.Log($"{solution}: {Solution.ProfileProjectName} (profile {profileFile}, extends {extends}), {name} (program), WinXL {version}");
        if (json)
        {
            Output.Json(writer =>
            {
                writer.WriteString("solution", solution);
                writer.WriteString("winxl", version);
                writer.WriteStartArray("projects");
                WriteProject(writer, Solution.ProfileProjectName, profileProject, "profile");
                WriteProject(writer, name, programProject, "program");
                writer.WriteEndArray();
            });
        }
        else
        {
            Console.Out.WriteLine(solution);
        }
        return 0;
    }

    public static string NewGuid() => Guid.NewGuid().ToString("B").ToUpperInvariant();

    public static void WriteProject(System.Text.Json.Utf8JsonWriter writer, string name, string path, string kind)
    {
        writer.WriteStartObject();
        writer.WriteString("name", name);
        writer.WriteString("path", path);
        writer.WriteString("kind", kind);
        writer.WriteEndObject();
    }
}
