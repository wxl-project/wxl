using System.Reflection;

namespace Wxl.Winxl;

/// <summary><c>winxl --version</c>: the version of the wxl tree this winxl was built from.</summary>
internal static class VersionCommand
{
    public static int Run(bool json)
    {
        var version = TreeVersion();
        if (json)
        {
            Output.Json(writer => writer.WriteString("version", version));
        }
        else
        {
            Console.Out.WriteLine(version);
        }
        return 0;
    }

    /// <summary>
    /// The informational version of the assembly, which winxl.csproj sets to WxlVersion,
    /// the version the build passes in, with no commit hash appended.
    /// </summary>
    private static string TreeVersion() =>
        typeof(VersionCommand).Assembly.GetCustomAttribute<AssemblyInformationalVersionAttribute>()?.InformationalVersion
        ?? throw new InvalidOperationException("winxl was built without an informational version");
}
