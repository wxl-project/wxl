using System.Text.Encodings.Web;
using System.Text.Json;

namespace Wxl.Winxl;

/// <summary>Where a command writes: the human log to stderr, the result to stdout.</summary>
internal static class Output
{
    /// <summary>A line of the human log.</summary>
    public static void Log(string line) => Console.Error.WriteLine(line);

    /// <summary>
    /// The one result object of a command under --json, written as UTF-8 to stdout
    /// itself rather than through the console's text encoding, so a path in another
    /// script reaches the reader intact whatever code page the console has. The
    /// relaxed encoder escapes the quote, the backslash and the control characters, which
    /// JSON requires, and leaves the rest as it is: the default one also escapes the
    /// characters HTML gives a meaning to (&lt; &gt; ' &amp;) and everything outside ASCII,
    /// which would leave paths and messages unreadable, and the output goes to a pipe or
    /// a file, never into a page.
    /// </summary>
    public static void Json(Action<Utf8JsonWriter> writeMembers)
    {
        var options = new JsonWriterOptions
        {
            Indented = true,
            Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping,
        };
        var stdout = Console.OpenStandardOutput();
        using (var writer = new Utf8JsonWriter(stdout, options))
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
