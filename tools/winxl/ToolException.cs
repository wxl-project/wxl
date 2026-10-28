namespace Wxl.Winxl;

/// <summary>How a command failed; the value is the exit code of the process.</summary>
internal enum Failure
{
    /// <summary>The arguments are not a command of winxl.</summary>
    Invocation = 1,

    /// <summary>A file the command was given, or a package it reads, is not what it has to be.</summary>
    Input = 2,

    /// <summary>The machine lacks or refuses what the command needs.</summary>
    Environment = 3,
}

/// <summary>A failure the command explains: its message says what is wrong and what to do.</summary>
internal sealed class ToolException(Failure failure, string message, Exception? inner = null)
    : Exception(message, inner)
{
    public Failure Failure { get; } = failure;

    /// <summary>The code of the failure in the JSON error object.</summary>
    public string Code => Failure switch
    {
        Failure.Invocation => "invocation",
        Failure.Input => "input",
        Failure.Environment => "environment",
        _ => throw new InvalidOperationException($"no code for the failure {Failure}"),
    };
}
