#pragma once

// What stands behind the properties wxl gives any element, or a control of one kind, that WinUI
// has no plain property for.
//
// Private: the element arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>

#include "hstring_param.h"
#include <wxl/FluentSymbol.h>
#include <wxl/Microsoft.UI.Input.Enums.h>

namespace wxl::impl {

/// `cursor = InputSystemCursorShape::SizeWestEast`: the shape the pointer takes over the element,
/// for the element's lifetime. WinUI keeps it on UIElement.ProtectedCursor; "protected" is a word
/// for projections, not for the ABI -- any element's object answers QueryInterface for
/// IUIElementProtected, so no subclass is needed.
void set_cursor(winrt::Microsoft::UI::Xaml::UIElement const& element, InputSystemCursorShape shape);

/// `toolTip = u"..."`: one text, two homes -- ToolTipService, the hover hint, and
/// AutomationProperties.Name, the name a screen reader speaks. The property exists for controls
/// whose face is a glyph rather than a word, and such a control needs naming in both places.
void set_tool_tip(winrt::Microsoft::UI::Xaml::UIElement const& element, hstring_param const& text);

/// `automationName = u"..."`: the name a screen reader speaks, and nothing else -- the half of
/// toolTip that carries no hover hint. A Button's accessible name comes from its content only when
/// that content is a string, so a list row built out of a panel is nameless to automation however
/// much text it shows; a hover hint there would only repeat what is on screen, so this is a
/// property of its own rather than a use of toolTip.
void set_automation_name(winrt::Microsoft::UI::Xaml::UIElement const& element,
                         hstring_param const& name);

/// The second spelling of SymbolIcon's Symbol: a FluentSymbol, every name the font's own
/// documentation carries, where WinRT's Symbol has 197 names from Windows 8. SymbolIcon takes a
/// code point in SymbolThemeFontFamily and nothing else, so the crossing is a cast, written here
/// once rather than at every call site.
void set_fluent_symbol(winrt::Microsoft::UI::Xaml::Controls::SymbolIcon const& icon,
                       FluentSymbol glyph);

/// What the header and the description of a settings card, and the header of a headered
/// control, count as "no text": null or an empty string -- the IsNullOrEmpty of the Toolkit's
/// SettingsCard, which then collapses the part.
inline bool is_blank(winrt::Windows::Foundation::IInspectable const& content) {
    if (!content) {
        return true;
    }
    if (auto const value = content.try_as<winrt::Windows::Foundation::IPropertyValue>()) {
        return value.Type() == winrt::Windows::Foundation::PropertyType::String &&
               value.GetString().empty();
    }
    return false;
}

}  // namespace wxl::impl
