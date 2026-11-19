#pragma once

// What stands behind wheelToParent, the wheel a ScrollViewer passes on.
//
// Private: the viewer arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace wxl::impl {

/// `wheelToParent = true` on a ScrollViewer. A viewer takes the wheel and marks it handled even
/// when it has nothing to scroll along it, so a horizontal code block stops the page around it.
/// With this on, a wheel turn the viewer cannot use -- no scrollable height, no modifier key held
/// -- goes on up the tree to the panel that scrolls the page. Added once, when the viewer is
/// built; false adds nothing.
void set_wheel_to_parent(winrt::Microsoft::UI::Xaml::Controls::ScrollViewer const& viewer, bool on);

}  // namespace wxl::impl
