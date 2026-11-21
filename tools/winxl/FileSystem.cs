using System.Text;
using System.Xml;
using System.Xml.Linq;

namespace Wxl.Winxl;

/// <summary>
/// How a text file is written: with or without the byte order mark of UTF-8, its line end, and
/// whether its last line ends with one. A file winxl changes keeps the form it had.
/// </summary>
internal sealed record FileFormat(bool ByteOrderMark, string NewLine, bool FinalNewLine)
{
    /// <summary>The form of the files winxl writes anew: no byte order mark, the line ends of Windows.</summary>
    public static readonly FileFormat New = new(false, "\r\n", true);

    /// <summary>The form of the file whose bytes are <paramref name="bytes"/>.</summary>
    public static FileFormat Of(byte[] bytes)
    {
        var bom = bytes.AsSpan().StartsWith(Encoding.UTF8.Preamble);
        var text = Encoding.UTF8.GetString(bytes);
        // The first line end decides; a file of one line takes that of Windows.
        var lf = text.IndexOf('\n');
        var newLine = lf > 0 && text[lf - 1] == '\r' || lf < 0 ? "\r\n" : "\n";
        return new FileFormat(bom, newLine, text.EndsWith('\n'));
    }
}

/// <summary>
/// The writes of <c>winxl new</c> and <c>winxl add</c>. A file or a directory the machine refuses
/// to write is a failure of the environment (exit code 3), as the contract in Program.cs says,
/// and not a fault of the tool.
/// </summary>
internal static class FileSystem
{
    /// <summary>Writes <paramref name="text"/> in the form of a new file.</summary>
    public static void Write(string path, string text)
    {
        var lines = text.Replace("\r\n", "\n", StringComparison.Ordinal).Replace("\n", "\r\n", StringComparison.Ordinal);
        Guard(path, () => File.WriteAllText(path, lines, new UTF8Encoding(encoderShouldEmitUTF8Identifier: false)));
    }

    /// <summary>Writes <paramref name="document"/> in the form <paramref name="format"/>.</summary>
    public static void Save(XDocument document, string path, FileFormat format)
    {
        var settings = new XmlWriterSettings
        {
            Encoding = new UTF8Encoding(format.ByteOrderMark),
            NewLineChars = format.NewLine,
            NewLineHandling = NewLineHandling.Replace,
            OmitXmlDeclaration = document.Declaration is null,
        };
        Guard(path, () =>
        {
            using (var stream = new FileStream(path, FileMode.Create, FileAccess.Write))
            {
                using (var writer = XmlWriter.Create(stream, settings))
                {
                    document.Save(writer);
                }
                // The XML writer ends the document at its last tag.
                if (format.FinalNewLine)
                {
                    stream.Write(Encoding.UTF8.GetBytes(format.NewLine));
                }
            }
        });
    }

    public static string CreateDirectory(string path)
    {
        var created = "";
        Guard(path, () => created = Directory.CreateDirectory(path).FullName);
        return created;
    }

    /// <summary>Whether <paramref name="path"/> is a directory with nothing in it.</summary>
    public static bool IsEmptyDirectory(string path)
    {
        var empty = false;
        Guard(path, () => empty = !Directory.EnumerateFileSystemEntries(path).Any());
        return empty;
    }

    public static byte[] Read(string path)
    {
        try
        {
            return File.ReadAllBytes(path);
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            throw new ToolException(Failure.Environment, $"{path} cannot be read: {e.Message}", e);
        }
    }

    /// <summary>
    /// Puts <paramref name="bytes"/> back into <paramref name="path"/> after a refused write: null
    /// when the file is as it was, or what the machine said when it could not be put back.
    /// </summary>
    public static string? Restore(string path, byte[] bytes)
    {
        try
        {
            if (!File.Exists(path) || !File.ReadAllBytes(path).AsSpan().SequenceEqual(bytes))
            {
                File.WriteAllBytes(path, bytes);
            }
            return null;
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            return $"{path} could not be restored and is left changed: {e.Message}";
        }
    }

    /// <summary>
    /// Removes the directory <paramref name="path"/> a refused command made: null when it is gone,
    /// or what the machine said when it could not be removed.
    /// </summary>
    public static string? Remove(string path)
    {
        try
        {
            if (Directory.Exists(path))
            {
                Directory.Delete(path, recursive: true);
            }
            return null;
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            return $"{path} could not be removed: {e.Message}";
        }
    }

    /// <summary>
    /// Deletes the file <paramref name="path"/> a refused command wrote: null when it is gone, or
    /// what the machine said when it could not be deleted.
    /// </summary>
    public static string? Delete(string path)
    {
        try
        {
            File.Delete(path);
            return null;
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            return $"{path} could not be deleted: {e.Message}";
        }
    }

    /// <summary>
    /// The failure <paramref name="failure"/> with what the undoing of the writes before it could not
    /// undo, so that the message, and the error object under --json, say what is left.
    /// </summary>
    public static ToolException Undone(ToolException failure, IEnumerable<string?> left)
    {
        var problems = left.OfType<string>().ToList();
        return problems.Count == 0
            ? failure
            : new ToolException(failure.Failure, $"{failure.Message}; undoing the writes before it: {string.Join("; ", problems)}", failure);
    }

    private static void Guard(string path, Action action)
    {
        try
        {
            action();
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            throw new ToolException(Failure.Environment, $"{path} cannot be written: {e.Message}", e);
        }
    }
}
