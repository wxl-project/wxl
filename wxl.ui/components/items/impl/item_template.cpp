#include "items_helpers.h"

#include "Object.impl.h"
#include <wxl/Microsoft.UI.Xaml.impl.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>
#include "impl/conversions.h"

namespace wxl::impl {

namespace xaml = winrt::Microsoft::UI::Xaml;
using winrt::Windows::Foundation::IInspectable;

void set_item_template(xaml::Controls::ListViewBase const& list, ItemBuilder const& build) {
    set_item_maker(list, [build](IInspectable const& item) -> IInspectable {
        return Object::Impl::as<xaml::UIElement>(build(Object::Impl::wrap<Object>(item)));
    });
}

void set_item_margin(xaml::Controls::ListViewBase const& list, Thickness const& margin) {
    set_container_margin(list, to_winrt(margin));
}

// The preset is worn by the container as the wrapper its setters are members of.
void set_item_container_style(xaml::Controls::ListViewBase const& list,
                              ItemContainerPreset const& preset) {
    set_container_dress(list, [dress = preset.dress()](
                                  xaml::Controls::Primitives::SelectorItem const& container) {
        dress(Object::Impl::wrap<SelectorItem>(container));
    });
}

}  // namespace wxl::impl
