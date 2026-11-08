#include "tool_tip_helpers.h"

#include "Object.impl.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>

namespace wxl::impl {

void set_tool_tip_element(winrt::Microsoft::UI::Xaml::UIElement const& element,
                          ToolTip const& tip) {
    winrt::Microsoft::UI::Xaml::Controls::ToolTipService::SetToolTip(
        element, *Object::Impl::get_typed<ToolTip>(tip));
}

}  // namespace wxl::impl
