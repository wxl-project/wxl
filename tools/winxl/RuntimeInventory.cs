// What winxl runtime install reads from a Microsoft.WindowsAppSDK.Runtime package --
// its MSIX inventory, and the identity in the manifest of each MSIX -- follows
// WindowsAppRuntimeService of winapp, the Windows App Development CLI
// (https://github.com/microsoft/winappcli, src/winapp-CLI/WinApp.Cli/Services/
// WindowsAppRuntimeService.cs at commit 31fc2ac52b9340846d9b5baa0b9be634ca77a8d1),
// rewritten for this tool.
//
// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License; the license text is in NOTICE next to this file.

using System.IO.Compression;
using System.Xml.Linq;
using Windows.System;

namespace Wxl.Winxl;

/// <summary>An MSIX of the runtime and the identity its manifest declares.</summary>
internal sealed record RuntimeMsix(
    string File,
    string Path,
    string Name,
    string Publisher,
    Version Version,
    ProcessorArchitecture Architecture,
    bool IsFramework)
{
    /// <summary>
    /// The DynamicDependency lifetime manager: the package an unpackaged application's
    /// bootstrapper looks for beside the framework.
    /// </summary>
    public bool IsLifetimeManager => Name.StartsWith("Microsoft.WinAppRuntime.DDLM.", StringComparison.OrdinalIgnoreCase);

    public string ArchitectureName => Architecture switch
    {
        ProcessorArchitecture.X64 => "x64",
        ProcessorArchitecture.X86 => "x86",
        ProcessorArchitecture.Arm64 => "arm64",
        ProcessorArchitecture.Neutral => "neutral",
        _ => throw new InvalidOperationException($"no name for the architecture {Architecture}"),
    };
}

/// <summary>
/// The inventory of the runtime's installers in a Microsoft.WindowsAppSDK.Runtime package,
/// <c>tools/MSIX/win10-&lt;arch&gt;/MSIX.inventory</c>: a line <c>&lt;file&gt;=&lt;identity&gt;</c> for each MSIX
/// of the directory.
/// </summary>
internal static class RuntimeInventory
{
    public const string FileName = "MSIX.inventory";

    private static readonly XNamespace Foundation = "http://schemas.microsoft.com/appx/manifest/foundation/windows10";

    /// <summary>The MSIX packages the inventory of <paramref name="directory"/> lists, in its order.</summary>
    public static IReadOnlyList<RuntimeMsix> Read(string directory)
    {
        var inventory = System.IO.Path.Combine(directory, FileName);
        if (!File.Exists(inventory))
        {
            throw new ToolException(Failure.Input, $"no {FileName} in {directory}");
        }

        var packages = new List<RuntimeMsix>();
        var number = 0;
        foreach (var line in File.ReadAllLines(inventory))
        {
            number++;
            if (string.IsNullOrWhiteSpace(line))
            {
                continue;
            }

            // Only the file name is taken: the identity the line gives can differ from
            // the one the package declares -- that of the DDLM does -- so the identity
            // comes from the manifest inside the file.
            var equals = line.IndexOf('=', StringComparison.Ordinal);
            var file = equals > 0 ? line[..equals].Trim() : "";
            if (file.Length == 0
                || file != System.IO.Path.GetFileName(file)
                || !file.EndsWith(".msix", StringComparison.OrdinalIgnoreCase))
            {
                throw new ToolException(Failure.Input,
                    $"{inventory}, line {number}: not <file>.msix=<identity> with a file of the directory: '{line}'");
            }
            packages.Add(ReadMsix(System.IO.Path.Combine(directory, file), file));
        }

        if (packages.Count == 0)
        {
            throw new ToolException(Failure.Input, $"{inventory} lists no package");
        }
        var twice = packages.GroupBy(p => p.Name, StringComparer.OrdinalIgnoreCase).FirstOrDefault(g => g.Count() > 1);
        if (twice is not null)
        {
            throw new ToolException(Failure.Input, $"{inventory} lists the package {twice.Key} more than once");
        }
        return packages;
    }

    private static RuntimeMsix ReadMsix(string path, string file)
    {
        if (!File.Exists(path))
        {
            throw new ToolException(Failure.Input, $"the inventory lists {file}, which is not in {System.IO.Path.GetDirectoryName(path)}");
        }

        XElement identity;
        XElement? properties;
        try
        {
            using var zip = ZipFile.OpenRead(path);
            var entry = zip.GetEntry("AppxManifest.xml")
                ?? throw new ToolException(Failure.Input, $"{path} has no AppxManifest.xml");
            using var stream = entry.Open();
            var package = XDocument.Load(stream).Root
                ?? throw new ToolException(Failure.Input, $"the AppxManifest.xml of {path} is empty");
            identity = package.Element(Foundation + "Identity")
                ?? throw new ToolException(Failure.Input, $"the AppxManifest.xml of {path} has no Identity");
            properties = package.Element(Foundation + "Properties");
        }
        catch (Exception e) when (e is InvalidDataException or System.Xml.XmlException)
        {
            throw new ToolException(Failure.Input, $"{path} is not an MSIX with a readable AppxManifest.xml: {e.Message}", e);
        }

        var name = Attribute(identity, "Name", path);
        var publisher = Attribute(identity, "Publisher", path);
        var versionText = Attribute(identity, "Version", path);
        if (!Version.TryParse(versionText, out var version) || version.Revision < 0)
        {
            throw new ToolException(Failure.Input, $"{path}: the version '{versionText}' is not <major>.<minor>.<build>.<revision>");
        }

        // A package without the attribute is neutral (the schema's default).
        var architectureText = (string?)identity.Attribute("ProcessorArchitecture") ?? "neutral";
        ProcessorArchitecture architecture = architectureText.ToLowerInvariant() switch
        {
            "x64" => ProcessorArchitecture.X64,
            "x86" => ProcessorArchitecture.X86,
            "arm64" => ProcessorArchitecture.Arm64,
            "neutral" => ProcessorArchitecture.Neutral,
            _ => throw new ToolException(Failure.Input, $"{path}: the architecture '{architectureText}' is not one winxl knows"),
        };

        var isFramework = string.Equals((string?)properties?.Element(Foundation + "Framework"), "true", StringComparison.OrdinalIgnoreCase);
        return new RuntimeMsix(file, path, name, publisher, version, architecture, isFramework);
    }

    private static string Attribute(XElement identity, string name, string path)
    {
        var value = (string?)identity.Attribute(name);
        return string.IsNullOrEmpty(value)
            ? throw new ToolException(Failure.Input, $"the Identity of the AppxManifest.xml of {path} has no {name}")
            : value;
    }
}
