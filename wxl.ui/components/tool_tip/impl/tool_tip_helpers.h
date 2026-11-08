#pragma once

// What stands behind toolTipElement, the tip of an element given as a ToolTip.
//
// Private: the element arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.h>

namespace wxl {
class ToolTip;
}

namespace wxl::impl {

/// The tip as an element of its own -- content that is not text, a placement, an offset. Only
/// the hover half: an element has no words to give a screen reader.
void set_tool_tip_element(winrt::Microsoft::UI::Xaml::UIElement const& element, ToolTip const& tip);

}  // namespace wxl::impl
