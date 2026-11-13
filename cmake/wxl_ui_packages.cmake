# What wxl.ui.common stands on in the restored NuGet packages:
#
#   wxl_ui_packages(<target>)
#
# made over the restore of cmake/wxl_packages.cmake, which is included before,
# and given to <target>: wxl.ui.common in the wxl tree, the imported
# wxl::ui.common in a project that takes an installed wxl
# (cmake/wxl-config.cmake.in). Both are paths into that restore, which differ
# from one build to the next, so an install does not carry them and every
# configure makes them anew. What is made:
#
#   wxl.winappsdk.bootstrap
#       a SHARED IMPORTED GLOBAL target over the bootstrapper of the Windows App
#       Runtime, which wxl.ui.common links PUBLIC. A target rather than the
#       path to its import library: CMake then knows the DLL behind it, and
#       $<TARGET_RUNTIME_DLLS> of an application names it without the
#       application knowing where the SDK lies. GLOBAL, so the name reaches the
#       directories where applications are declared; PUBLIC, because the DLL
#       has to reach their folders and not only wxl's link line.
#
#   WXL_RUNTIME_FILES of <target>
#       the DLLs nothing links that wxl_target_runtime() puts beside an
#       application: Win2D, which activates by name at run time and carries no
#       import library, and WebView2's loader.
#
#   WXL_UI_PROFILE_LIBRARIES and WXL_UI_PROFILE_INCLUDE_DIRECTORIES of <target>
#       what the profile layer builds against and no component owns, PRIVATE
#       to a library of a profile like the projection (wxl_add_ui_library): an
#       application links none of it itself.
#
# A package without the file wxl takes from it is said as the tree has always
# said it: the bootstrapper and the DLLs with a warning naming what fails
# without them, the header of the release with a stop. A second call -- a second
# find_package(wxl), which reads the configuration again -- sets the same
# properties again and keeps the target the first one made.

include_guard(GLOBAL)

function(wxl_ui_packages target)
    wxl_nuget_package_dir(foundation Microsoft.WindowsAppSDK.Foundation)
    wxl_nuget_package_dir(winui Microsoft.WindowsAppSDK.WinUI)
    wxl_nuget_package_dir(win2d Microsoft.Graphics.Win2D)
    wxl_nuget_package_dir(webview2 Microsoft.Web.WebView2)

    # The release an application starts on is the one the restore resolved:
    # impl/bootstrap.cpp includes include/WindowsAppSDK-VersionInfo.h of the
    # restored Microsoft.WindowsAppSDK.Runtime package and hands the
    # bootstrapper the version tag and the major version that name the
    # release's framework package (WINDOWSAPPSDK_RELEASE_VERSION_TAG_W,
    # WINDOWSAPPSDK_RELEASE_MAJORMINOR), so no value of the release is kept
    # here. The installers of that framework package are in the same Runtime
    # package, and winxl runtime install (tools/winxl) installs them: the
    # release of the tree is an experimental one (tools/packages.json), and a
    # stable runtime answers a call to WrapPanel, TitleBar.RecomputeDragRegions
    # or the Window size properties with "no such interface".
    wxl_nuget_package_dir(runtime Microsoft.WindowsAppSDK.Runtime)
    if(NOT EXISTS "${runtime}/include/WindowsAppSDK-VersionInfo.h")
        message(FATAL_ERROR
            "No include/WindowsAppSDK-VersionInfo.h in ${runtime}, the restored "
            "Microsoft.WindowsAppSDK.Runtime package: the bootstrapper of wxl takes the release it "
            "starts from that header.")
    endif()

    if(TARGET wxl.winappsdk.bootstrap)
        # Made by the first call.
    elseif(EXISTS "${foundation}/lib/native/x64/Microsoft.WindowsAppRuntime.Bootstrap.lib")
        add_library(wxl.winappsdk.bootstrap SHARED IMPORTED GLOBAL)
        set_target_properties(wxl.winappsdk.bootstrap PROPERTIES
            IMPORTED_LOCATION
                "${foundation}/runtimes/win-x64/native/Microsoft.WindowsAppRuntime.Bootstrap.dll"
            IMPORTED_IMPLIB
                "${foundation}/lib/native/x64/Microsoft.WindowsAppRuntime.Bootstrap.lib"
            # Whoever compiles impl/bootstrap.cpp -- wxl.ui.common, or a sandbox that
            # takes the file directly -- asks for the same release, the one of this header.
            INTERFACE_INCLUDE_DIRECTORIES
                "${runtime}/include")
    else()
        message(WARNING
            "No Microsoft.WindowsAppRuntime.Bootstrap.lib at ${foundation}; "
            "applications built on wxl will fail to start.")
    endif()

    set(runtime_files "")
    if(EXISTS "${win2d}/runtimes/win-x64/native/Microsoft.Graphics.Canvas.dll")
        list(APPEND runtime_files "${win2d}/runtimes/win-x64/native/Microsoft.Graphics.Canvas.dll")
    else()
        message(WARNING
            "No Microsoft.Graphics.Canvas.dll at ${win2d}; a CompositionWindow "
            "will fault while building its shared CanvasDevice.")
    endif()
    if(EXISTS "${webview2}/runtimes/win-x64/native/WebView2Loader.dll")
        list(APPEND runtime_files "${webview2}/runtimes/win-x64/native/WebView2Loader.dll")
    else()
        message(WARNING
            "No WebView2Loader.dll at ${webview2}; a MapControl or a WebView2 will end the process "
            "when it is shown.")
    endif()

    set(libraries
        Microsoft::CppWinRT
        runtimeobject.lib
        # The graphics stack under a composition drawing surface: a D3D11
        # device, the DXGI device inside it and the D2D device on top --
        # see DrawingSurface.cpp. An application draws through the
        # ID2D1DeviceContext it is handed and links none of this itself.
        d3d11.lib
        d2d1.lib
        dxgi.lib
        # WIC, decoding the backdrop image -- see WindowNative.cpp.
        windowscodecs.lib
        # The shell, twice over: files dropped on a window (DragAcceptFiles,
        # DragQueryFileW in WindowNative.cpp, with the window subclass that
        # catches the message they arrive in) and the notification area
        # (Shell_NotifyIconW in TrayIcon.cpp).
        shell32.lib
        comctl32.lib
        # Painting the redirection surface behind the island -- the one
        # spot where GDI is the right tool; see WindowNative.cpp.
        gdi32.lib
        # CreateDispatcherQueueController, for the system compositor a
        # WindowShade hosts on its own overlay window -- see WindowShade.cpp.
        coremessaging.lib
        # DwmSetWindowAttribute: the dark frame and the caption colour of a
        # CompositionWindow follow the application's theme.
        dwmapi.lib
    )
    set_target_properties(${target} PROPERTIES
        WXL_RUNTIME_FILES "${runtime_files}"
        WXL_UI_PROFILE_LIBRARIES "${libraries}"
        # The package headers the profile layer includes, PRIVATE to it as to
        # wxl.ui.common: WinUI's native interop headers and the bootstrapper's.
        WXL_UI_PROFILE_INCLUDE_DIRECTORIES "${foundation}/include;${winui}/include")
endfunction()
