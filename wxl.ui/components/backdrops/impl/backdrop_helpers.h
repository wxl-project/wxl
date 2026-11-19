#pragma once

// What stands behind `micaController = ...`, `desktopAcrylicController = ...` and
// `noSystemBackdrop = true` on a Window. A system backdrop controller is told which window to
// draw under by the window's ICompositionSupportsSystemBackdrop, an interface no wrapper declares
// and the window answers to QueryInterface alone; the controller then needs its configuration
// and, when the window closes, Close.
//
// Private: the window arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.h>

namespace wxl {
class MicaController;
class DesktopAcrylicController;
}  // namespace wxl

namespace wxl::impl {

void clear_system_backdrop(winrt::Microsoft::UI::Xaml::Window const& window, bool clear);

void set_mica_controller(winrt::Microsoft::UI::Xaml::Window const& window,
                         MicaController const& controller);

void set_desktop_acrylic_controller(winrt::Microsoft::UI::Xaml::Window const& window,
                                    DesktopAcrylicController const& controller);

}  // namespace wxl::impl
