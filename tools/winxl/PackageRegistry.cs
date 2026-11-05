// The calls of the Windows package manager follow PackageRegistrationService of
// winapp, the Windows App Development CLI (https://github.com/microsoft/winappcli,
// src/winapp-CLI/WinApp.Cli/Services/PackageRegistrationService.cs at commit
// 31fc2ac52b9340846d9b5baa0b9be634ca77a8d1), rewritten for this tool.
//
// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License; the license text is in NOTICE next to this file.

using Windows.Foundation;
using Windows.Management.Deployment;

namespace Wxl.Winxl;

/// <summary>
/// The packages of the current user, through the WinRT API of the Windows package manager
/// (Windows.Management.Deployment.PackageManager) rather than PowerShell's Appx module.
/// </summary>
internal sealed class PackageRegistry
{
    private readonly PackageManager manager = new();

    /// <summary>
    /// The highest version of <paramref name="msix"/>'s package -- its name, publisher and
    /// architecture -- registered for the current user, or null. Several versions of a
    /// package can be registered at once, in no promised order.
    /// </summary>
    public Version? RegisteredVersion(RuntimeMsix msix)
    {
        Version? highest = null;
        try
        {
            // An empty security identifier is the current user.
            foreach (var package in manager.FindPackagesForUser(string.Empty, msix.Name, msix.Publisher))
            {
                var id = package.Id;
                if (id.Architecture != msix.Architecture)
                {
                    continue;
                }
                var version = new Version(id.Version.Major, id.Version.Minor, id.Version.Build, id.Version.Revision);
                if (highest is null || version > highest)
                {
                    highest = version;
                }
            }
        }
        catch (Exception e) when (e is not ToolException)
        {
            throw new ToolException(Failure.Environment,
                $"the package manager could not list the packages {msix.Name} of the current user: {e.Message} (0x{e.HResult:X8})", e);
        }
        return highest;
    }

    /// <summary>
    /// Installs <paramref name="msix"/> for the current user, as Add-AppxPackage does without
    /// options: an application that uses an older version is not shut down for it.
    /// </summary>
    public void Install(RuntimeMsix msix)
    {
        IAsyncOperationWithProgress<DeploymentResult, DeploymentProgress> operation;
        try
        {
            operation = manager.AddPackageAsync(new Uri(msix.Path), null, DeploymentOptions.None);
        }
        catch (Exception e) when (e is not ToolException)
        {
            throw new ToolException(Failure.Environment,
                $"the package manager could not start the installation of {msix.File}: {e.Message} (0x{e.HResult:X8})", e);
        }

        DeploymentResult result;
        try
        {
            // A blocking wait on the main thread: nothing continues on the thread that
            // completes the operation, which is what the deployment stack waits for.
            result = operation.AsTask().GetAwaiter().GetResult();
        }
        catch (Exception e) when (e is not ToolException)
        {
            // A failed deployment completes with AsyncStatus.Error, and the task faults
            // with the generic text of its HRESULT; what the deployment says -- the text
            // Add-AppxPackage prints -- is the ErrorText of its result.
            throw new ToolException(Failure.Environment,
                $"the package manager could not install {msix.File}: {DeploymentErrorText(operation, e)} (0x{e.HResult:X8})", e);
        }
        if (!string.IsNullOrEmpty(result.ErrorText))
        {
            var code = result.ExtendedErrorCode is { } extended ? $" (0x{extended.HResult:X8})" : "";
            throw new ToolException(Failure.Environment,
                $"the package manager could not install {msix.File}: {result.ErrorText}{code}");
        }
    }

    /// <summary>
    /// The ErrorText of a failed deployment, or the message of <paramref name="failure"/>
    /// when the operation has none to give: the failure is reported either way, and only
    /// its wording depends on this.
    /// </summary>
    private static string DeploymentErrorText(
        IAsyncOperationWithProgress<DeploymentResult, DeploymentProgress> operation, Exception failure)
    {
        if (operation.Status != AsyncStatus.Error)
        {
            return failure.Message;
        }
        try
        {
            var text = operation.GetResults()?.ErrorText;
            return string.IsNullOrEmpty(text) ? failure.Message : text;
        }
        catch (Exception e) when (e is not ToolException)
        {
            return failure.Message;
        }
    }
}
