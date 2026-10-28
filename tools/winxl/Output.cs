using System.Text.Json;

namespace Wxl.Winxl;

/// <summary>Where a command writes: the human log to stderr, the result to stdout.</summary>
internal static class Output
{
    /// <summary>A line of the human log.</summary>
    public static void Log(string line) => Console.Error.WriteLine(line);

    /// <summary>
    /// The one result object of a command under --json, written as UTF-8 to stdout
    /// itself rather than through the console's text encoding. The writer's default
    /// encoder escapes every character outside ASCII, so a path in another script
    /// reaches the reader intact whatever code page the console has.
    /// </summary>
    public static void Json(Action<Utf8JsonWriter> writeMembers)
    {
        var stdout = Console.OpenStandardOutput();
        using (var writer = new Utf8JsonWriter(stdout, new JsonWriterOptions { Indented = true }))
        {
            writer.WriteStartObject();
            writeMembers(writer);
            writer.WriteEndObject();
        }
        stdout.WriteByte((byte)'\n');
        stdout.Flush();
    }

    public static void Error(ToolException error, bool json)
    {
        Console.Error.WriteLine($"winxl: {error.Message}");
        if (json)
        {
            Json(writer =>
            {
                writer.WriteStartObject("error");
                writer.WriteString("code", error.Code);
                writer.WriteString("message", error.Message);
                writer.WriteEndObject();
            });
        }
    }
}
