// The order of the installation, the skip of a package registered at its version or
// newer, and the check that follows it come from WindowsAppRuntimeService of winapp,
// the Windows App Development CLI (https://github.com/microsoft/winappcli,
// src/winapp-CLI/WinApp.Cli/Services/WindowsAppRuntimeService.cs at commit
// 31fc2ac52b9340846d9b5baa0b9be634ca77a8d1), rewritten for this tool.
//
// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License; the license text is in NOTICE next to this file.

namespace Wxl.Winxl;

/// <summary>
/// <c>winxl runtime install --packages &lt;list&gt; [--json]</c>: installs, for the current user,
/// the Windows App Runtime of the Microsoft.WindowsAppSDK.Runtime package the restore of a
/// build resolved.
/// </summary>
/// <remarks>
/// Why: the WinUI metadata the wrappers of wxl are generated from is that of the
/// experimental channel, and only the experimental framework package
/// (Microsoft.WindowsAppRuntime.2-experimentalF) has what it declares -- the stable runtime
/// answers a call to WrapPanel, to the Window size properties or to
/// TitleBar.RecomputeDragRegions with "no such interface". An application started by wxl
/// asks for that framework by the version tag of include/WindowsAppSDK-VersionInfo.h in the
/// same Runtime package (wxl.ui/src/impl/bootstrap.cpp). It is installed next to the
/// stable runtime, not instead of it: other applications keep theirs.
///
/// The installers are those of <c>tools/MSIX/win10-x64</c>: the applications the tree builds
/// are x64 (CMakePresets.json), and an x64 application asks for the x64 framework and
/// lifetime manager on any Windows.
/// </remarks>
internal static class RuntimeInstallCommand
{
    private const string RuntimePackageId = "Microsoft.WindowsAppSDK.Runtime";
    private const string InstallerDirectory = "tools/MSIX/win10-x64";

    private sealed record Outcome(RuntimeMsix Msix, bool Installed, Version Registered);

    public static int Run(string packagesList, bool json)
    {
        var runtime = RestoredPackages.Find(packagesList, RuntimePackageId);
        var packageDirectory = Path.GetFullPath(runtime.Directory);
        if (!Directory.Exists(packageDirectory))
        {
            throw new ToolException(Failure.Environment,
                $"{RuntimePackageId} {runtime.Version} is not in {packageDirectory}, where the list of the "
                + "restore says it is: configure the build again, and run winxl after its restore");
        }
        var installers = Path.Combine(packageDirectory, InstallerDirectory);
        if (!Directory.Exists(installers))
        {
            throw new ToolException(Failure.Input,
                $"no installers at {installers}, in {RuntimePackageId} {runtime.Version}");
        }
        Output.Log($"{RuntimePackageId} {runtime.Version}: {installers}");

        var inventory = RuntimeInventory.Read(installers);
        if (!inventory.Any(msix => msix.IsFramework))
        {
            throw new ToolException(Failure.Input, $"the inventory of {installers} has no framework package");
        }
        if (!inventory.Any(msix => msix.IsLifetimeManager))
        {
            throw new ToolException(Failure.Input,
                $"the inventory of {installers} has no DynamicDependency lifetime manager (Microsoft.WinAppRuntime.DDLM.*)");
        }

        // The framework first, the packages that depend on it after -- the main and
        // singleton packages and the lifetime manager each declare a dependency on it --
        // and otherwise in the order of the inventory.
        var ordered = inventory.Where(msix => msix.IsFramework).Concat(inventory.Where(msix => !msix.IsFramework)).ToList();

        var registry = new PackageRegistry();
        var installed = new HashSet<RuntimeMsix>();
        foreach (var msix in ordered)
        {
            var registered = registry.RegisteredVersion(msix);
            if (registered is not null && registered >= msix.Version)
            {
                Output.Log($"skip {msix.File}: {msix.Name} {registered} is registered");
                continue;
            }
            Output.Log($"install {msix.File}: {msix.Name} {msix.Version} {msix.ArchitectureName}");
            registry.Install(msix);
            installed.Add(msix);
        }

        // The package manager answered every installation; what an application finds at
        // its start is what is registered, so that is what is checked.
        var outcomes = new List<Outcome>();
        foreach (var msix in ordered)
        {
            var registered = registry.RegisteredVersion(msix);
            if (registered is null || registered < msix.Version)
            {
                throw new ToolException(Failure.Environment,
                    $"after the installation, {msix.Name} {msix.ArchitectureName} is registered for the current user at "
                    + $"{registered?.ToString() ?? "no version"}, not at {msix.Version} or newer");
            }
            outcomes.Add(new Outcome(msix, installed.Contains(msix), registered));
        }

        if (json)
        {
            Output.Json(writer =>
            {
                writer.WriteStartObject("runtime");
                writer.WriteString("id", runtime.Id);
                writer.WriteString("version", runtime.Version);
                writer.WriteString("directory", packageDirectory);
                writer.WriteEndObject();

                writer.WriteStartArray("packages");
                foreach (var outcome in outcomes)
                {
                    writer.WriteStartObject();
                    writer.WriteString("file", outcome.Msix.File);
                    writer.WriteString("name", outcome.Msix.Name);
                    writer.WriteString("version", outcome.Msix.Version.ToString());
                    writer.WriteString("architecture", outcome.Msix.ArchitectureName);
                    writer.WriteBoolean("framework", outcome.Msix.IsFramework);
                    writer.WriteString("action", outcome.Installed ? "installed" : "skipped");
                    writer.WriteString("registered", outcome.Registered.ToString());
                    writer.WriteEndObject();
                }
                writer.WriteEndArray();
            });
        }
        else
        {
            foreach (var outcome in outcomes)
            {
                Console.Out.WriteLine($"{outcome.Msix.Name} {outcome.Registered} {outcome.Msix.ArchitectureName}");
            }
        }
        return 0;
    }
}
