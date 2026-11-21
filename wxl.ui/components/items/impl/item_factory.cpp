#include "items_helpers.h"

#include "Object.impl.h"
#include <wxl/Microsoft.UI.Xaml.impl.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>

namespace wxl::impl {

namespace {

namespace xaml = winrt::Microsoft::UI::Xaml;
using winrt::Windows::Foundation::IInspectable;

winrt_item_builder maker_of(ItemBuilder const& build) {
    return [build](IInspectable const& item) -> IInspectable {
        return Object::Impl::as<xaml::UIElement>(build(Object::Impl::wrap<Object>(item)));
    };
}

}  // namespace

void set_repeater_template(xaml::Controls::ItemsRepeater const& repeater,
                           ItemBuilder const& build) {
    repeater.ItemTemplate(element_factory_of(maker_of(build), false));
}

void set_items_view_template(xaml::Controls::ItemsView const& view, ItemBuilder const& build) {
    view.ItemTemplate(element_factory_of(maker_of(build), true));
}

}  // namespace wxl::impl
