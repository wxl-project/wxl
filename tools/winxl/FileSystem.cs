using System.Text;
using System.Xml;
using System.Xml.Linq;

namespace Wxl.Winxl;

/// <summary>
/// The writes of <c>winxl new</c> and <c>winxl add</c>. A file or a directory the machine refuses
/// to write is a failure of the environment (exit code 3), as the contract in Program.cs says,
/// and not a fault of the tool.
/// </summary>
internal static class FileSystem
{
    /// <summary>Writes <paramref name="text"/> as UTF-8 without a byte order mark, with the line ends of Windows.</summary>
    public static void Write(string path, string text)
    {
        var lines = text.Replace("\r\n", "\n", StringComparison.Ordinal).Replace("\n", "\r\n", StringComparison.Ordinal);
        Guard(path, () => File.WriteAllText(path, lines, new UTF8Encoding(encoderShouldEmitUTF8Identifier: false)));
    }

    /// <summary>Writes <paramref name="document"/> as UTF-8 without a byte order mark, with the line ends of Windows.</summary>
    public static void Save(XDocument document, string path)
    {
        var settings = new XmlWriterSettings
        {
            Encoding = new UTF8Encoding(encoderShouldEmitUTF8Identifier: false),
            NewLineChars = "\r\n",
            NewLineHandling = NewLineHandling.Replace,
            OmitXmlDeclaration = document.Declaration is null,
        };
        Guard(path, () =>
        {
            using var writer = XmlWriter.Create(path, settings);
            document.Save(writer);
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
    /// Puts <paramref name="bytes"/> back into <paramref name="path"/> after a refused write, as far
    /// as the machine lets it: the failure being reported is the one that matters.
    /// </summary>
    public static void Restore(string path, byte[] bytes)
    {
        try
        {
            if (!File.Exists(path) || !File.ReadAllBytes(path).AsSpan().SequenceEqual(bytes))
            {
                File.WriteAllBytes(path, bytes);
            }
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            Output.Log($"{path} could not be restored: {e.Message}");
        }
    }

    /// <summary>Removes the directory <paramref name="path"/> a refused command made, as far as the machine lets it.</summary>
    public static void Remove(string path)
    {
        try
        {
            if (Directory.Exists(path))
            {
                Directory.Delete(path, recursive: true);
            }
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            Output.Log($"{path} could not be removed: {e.Message}");
        }
    }

    /// <summary>Deletes the file <paramref name="path"/> a refused command wrote, as far as the machine lets it.</summary>
    public static void Delete(string path)
    {
        try
        {
            File.Delete(path);
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            Output.Log($"{path} could not be deleted: {e.Message}");
        }
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
