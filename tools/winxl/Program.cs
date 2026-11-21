// winxl -- the command-line utility of the wxl tree.
//
// The contract every command keeps:
//
//   - arguments are named: after the words of the command, only --<name> and
//     --<name> <value>, each at most once, in any order;
//   - --json prints one result object on stdout, and the human log goes to stderr;
//     without it the log goes to stderr as well, and stdout carries the result as
//     text;
//   - the exit code says how the command ended: 0 done, 1 wrong invocation, 2 bad
//     input (a file the command was given, or a package it reads), 3 environment
//     (what the machine lacks or refuses: a package directory, the Windows package
//     manager, a file or directory the command writes -- what it wrote before is
//     undone, and what it could not undo is in the message --, the headers of the
//     package winxl is part of, of the Windows SDK and of the MSVC toolsets it checks
//     names against);
//   - an error is a message on stderr and, under --json, the object
//     { "error": { "code": "<invocation|input|environment>", "message": "..." } }
//     on stdout, its code naming the exit code.
//
// Unexpected exceptions -- a fault of the tool, not of its input or of the machine --
// are not caught: the process stops with them, and with a nonzero code.

namespace Wxl.Winxl;

internal static class Program
{
    private const string Usage =
        """
        usage:
          winxl --version [--json]
              the version of the wxl tree this winxl was built from
          winxl runtime install --packages <build>/wxl.packages/restored-packages.json [--json]
              installs, for the current user, the Windows App Runtime of the
              Microsoft.WindowsAppSDK.Runtime package the restore of that build resolved
          winxl new wxl --name <name> [--dir <directory>] [--extends <profile>] [--json]
              writes into <directory> (./<name> by default) the MSBuild solution
              <name>.slnx of an application on wxl: the profile project wxl, whose
              profile <name>.winxl-profile extends <profile> of the package WinXL
              (base by default), and the program <name> on it
          winxl add library --name <name> [--ui] [--references <library>,...]
                            [--solution <solution>] [--json]
              adds the static library <name> to that solution, referenced by its
              program; with --ui the library stands on the profile of the solution,
              without it on wxl.core alone; --references names the libraries of the
              solution it uses. The solution is <solution>, or the one .slnx in the
              current directory
          winxl --help
        """;

    private static int Main(string[] args)
    {
        // Known before the arguments are read, so that a wrong invocation is
        // reported in the form it asked for.
        var json = args.Contains("--json", StringComparer.Ordinal);
        try
        {
            return Run(args, json);
        }
        catch (ToolException e)
        {
            Output.Error(e, json);
            if (e.Failure == Failure.Invocation && !json)
            {
                Console.Error.WriteLine(Usage);
            }
            return (int)e.Failure;
        }
    }

    private static int Run(string[] args, bool json)
    {
        if (args.Length == 0)
        {
            throw new ToolException(Failure.Invocation, "no command");
        }

        switch (args[0])
        {
            case "--help":
                if (args.Length > 1)
                {
                    throw new ToolException(Failure.Invocation, HelpAlone);
                }
                Console.Out.WriteLine(Usage);
                return 0;

            case "--version":
                Options.Parse(args, flags: ["--version", "--json"]);
                return VersionCommand.Run(json);

            case "runtime" when args.Length > 1 && args[1] == "install":
                var options = Options.Parse(args.AsSpan(2), flags: ["--json"], valued: ["--packages"]);
                var packages = options.Value("--packages")
                    ?? throw new ToolException(Failure.Invocation,
                        "runtime install needs --packages <build>/wxl.packages/restored-packages.json, "
                        + "the list of the restored packages the configure of a build writes");
                return RuntimeInstallCommand.Run(packages, json);

            case "new" when args.Length > 1 && !args[1].StartsWith("--", StringComparison.Ordinal):
                var created = Options.Parse(args.AsSpan(2), flags: ["--json"], valued: ["--name", "--dir", "--extends"]);
                return NewCommand.Run(args[1],
                    created.Value("--name") ?? throw new ToolException(Failure.Invocation, "new needs --name <name>, the name of the solution and of its program"),
                    created.Value("--dir"), created.Value("--extends"), json);

            case "add" when args.Length > 1 && args[1] == "library":
                var added = Options.Parse(args.AsSpan(2), flags: ["--json", "--ui"], valued: ["--name", "--references", "--solution"]);
                return AddLibraryCommand.Run(
                    added.Value("--name") ?? throw new ToolException(Failure.Invocation, "add library needs --name <name>, the name of the library"),
                    added.Has("--ui"), added.Value("--references"), added.Value("--solution"), json);

            default:
                throw new ToolException(Failure.Invocation,
                    Misplaced(args) ?? $"'{string.Join(' ', args)}' is not a command of winxl");
        }
    }

    private const string HelpAlone = "--help takes no other argument: winxl --help";

    /// <summary>
    /// What to say when an argument comes before the command it belongs to, as in
    /// <c>winxl --json runtime install ...</c>: the arguments go after the command, and the
    /// message says where; null when the arguments name no command at all.
    /// </summary>
    private static string? Misplaced(string[] args)
    {
        var first = args[0];
        if (!first.StartsWith("--", StringComparison.Ordinal))
        {
            return null;
        }
        if (args.Contains("--help", StringComparer.Ordinal))
        {
            return HelpAlone;
        }
        if (args.Contains("--version", StringComparer.Ordinal))
        {
            return $"{first} goes after the command: winxl --version {first}";
        }
        foreach (var (word, next) in new[] { ("runtime", "install"), ("new", Solution.Kind), ("add", "library") })
        {
            var at = Array.IndexOf(args, word);
            if (at > 0 && at + 1 < args.Length && args[at + 1] == next)
            {
                return $"{first} goes after the command: winxl {word} {next} ... {first}";
            }
        }
        return null;
    }
}

/// <summary>The named arguments of a command: flags, and options with a value.</summary>
internal sealed class Options
{
    private readonly Dictionary<string, string?> values = new(StringComparer.Ordinal);

    public static Options Parse(ReadOnlySpan<string> args, string[] flags, string[]? valued = null)
    {
        var options = new Options();
        for (var i = 0; i < args.Length; i++)
        {
            var name = args[i];
            if (!name.StartsWith("--", StringComparison.Ordinal))
            {
                throw new ToolException(Failure.Invocation,
                    $"'{name}': winxl takes named arguments only, --<name> or --<name> <value>");
            }
            if (options.values.ContainsKey(name))
            {
                throw new ToolException(Failure.Invocation, $"{name} is given twice");
            }
            if (flags.Contains(name, StringComparer.Ordinal))
            {
                options.values[name] = null;
                continue;
            }
            if (valued is null || !valued.Contains(name, StringComparer.Ordinal))
            {
                throw new ToolException(Failure.Invocation, $"{name} is not an argument of this command");
            }
            if (i + 1 == args.Length || args[i + 1].StartsWith("--", StringComparison.Ordinal))
            {
                throw new ToolException(Failure.Invocation, $"{name} needs a value");
            }
            options.values[name] = args[++i];
        }
        return options;
    }

    public string? Value(string name) => values.TryGetValue(name, out var value) ? value : null;

    public bool Has(string name) => values.ContainsKey(name);
}
