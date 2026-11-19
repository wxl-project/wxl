#include <winrt/Microsoft.UI.Xaml.Markup.h>

#include "items_helpers.h"

namespace wxl::impl {

namespace controls = winrt::Microsoft::UI::Xaml::Controls;

void set_items_panel_orientation(winrt::Microsoft::UI::Xaml::Controls::ItemsControl const& control,
                                 Orientation orientation) {
    namespace xaml = winrt::Microsoft::UI::Xaml;

    // Both the text and the answer of the loader are short-lived: the control keeps the template.
    winrt::hstring const text =
        orientation == Orientation::Vertical
            ? L"<ItemsPanelTemplate "
              L"xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>"
              L"<VirtualizingStackPanel Orientation='Vertical'/></ItemsPanelTemplate>"
            : L"<ItemsPanelTemplate "
              L"xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>"
              L"<VirtualizingStackPanel Orientation='Horizontal'/></ItemsPanelTemplate>";
    control.ItemsPanel(
        xaml::Markup::XamlReader::Load(text).as<xaml::Controls::ItemsPanelTemplate>());
}

void set_element_items(winrt::Microsoft::UI::Xaml::Controls::TreeView const& control, bool on) {
    namespace xaml = winrt::Microsoft::UI::Xaml;

    if (!on) {
        control.ItemTemplate(nullptr);
        return;
    }
    control.ItemTemplate(
        xaml::Markup::XamlReader::Load(
            L"<DataTemplate xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>"
            L"<ContentControl Content='{Binding Content}' "
            L"HorizontalContentAlignment='Left'/></DataTemplate>")
            .as<xaml::DataTemplate>());
}

// A SwipeControl has no items until someone gives it some: the collection is
// made here, empty, for the items to be added to afterwards.
void set_left_items_mode(controls::SwipeControl const& control, SwipeMode mode) {
    if (!control.LeftItems()) {
        control.LeftItems(controls::SwipeItems{});
    }
    control.LeftItems().Mode(static_cast<controls::SwipeMode>(mode));
}

void set_right_items_mode(controls::SwipeControl const& control, SwipeMode mode) {
    if (!control.RightItems()) {
        control.RightItems(controls::SwipeItems{});
    }
    control.RightItems().Mode(static_cast<controls::SwipeMode>(mode));
}

}  // namespace wxl::impl
