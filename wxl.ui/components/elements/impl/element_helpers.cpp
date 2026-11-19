#include "element_helpers.h"

#include "impl/conversions.h"

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>

namespace wxl::impl {

void set_cursor(winrt::Microsoft::UI::Xaml::UIElement const& element,
                InputSystemCursorShape const shape) {
    // as<>() rather than try_as<>(): an element whose object does not answer
    // for IUIElementProtected is a broken runtime, not a case to handle.
    element.as<winrt::Microsoft::UI::Xaml::IUIElementProtected>().ProtectedCursor(
        winrt::Microsoft::UI::Input::InputSystemCursor::Create(
            static_cast<winrt::Microsoft::UI::Input::InputSystemCursorShape>(shape)));
}

void set_tool_tip(winrt::Microsoft::UI::Xaml::UIElement const& element, hstring_param const& name) {
    winrt::hstring const& value = to_winrt(name);
    winrt::Microsoft::UI::Xaml::Controls::ToolTipService::SetToolTip(element,
                                                                     winrt::box_value(value));
    winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::SetName(element, value);
}

void set_automation_name(winrt::Microsoft::UI::Xaml::UIElement const& element,
                         hstring_param const& name) {
    winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::SetName(element, to_winrt(name));
}

void set_fluent_symbol(winrt::Microsoft::UI::Xaml::Controls::SymbolIcon const& icon,
                       FluentSymbol glyph) {
    // Both enumerations are int32_t code points into the same font, and the
    // ranges do not even meet -- Symbol sits in E1xx, FluentSymbol from E700
    // up -- so nothing is being reinterpreted here beyond the name.
    icon.Symbol(static_cast<winrt::Microsoft::UI::Xaml::Controls::Symbol>(glyph));
}

}  // namespace wxl::impl
