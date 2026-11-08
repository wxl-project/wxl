#pragma once

// What stands behind autoSuggestBox, the search box of a NavigationView.
//
// Private: the view arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include "core.h"
// AutoSuggestBox complete: core::nullable of an incomplete class picks the raw-pointer form and
// keeps it for the rest of the translation unit.
#include <wxl/Microsoft.UI.Xaml.Controls.h>

namespace wxl::impl {

/// `autoSuggestBox = ...` on a NavigationView: the search box of the pane, which the framework
/// lets be none. A wrapper cannot be empty, so the property takes a core::nullable and an empty
/// one clears the box.
void set_auto_suggest_box(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& view,
                          core::nullable<AutoSuggestBox> const& box);

}  // namespace wxl::impl
