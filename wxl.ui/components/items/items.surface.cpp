// Compile-time check of the declarative surface of the list controls.
// Built into the surface test of every profile that selects the component;
// nothing here runs.

#include "ui.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// itemContainerStyle -- a synthetic member: the preset every container wears, once, as the control makes it.
[[maybe_unused]] void ListViewBase_itemContainerStyle_preset() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    GridView{itemContainerStyle = Preset{margin = Thickness{0, 0, 12, 12},
                                         horizontalContentAlignment = HorizontalAlignment::Stretch},
             TextBlock{u"item"}};
    ListView{itemContainerStyle = Preset{padding = Thickness{4}}, TextBlock{u"item"}};
}

// boundTemplate -- a DataTemplate whose root is a control and whose properties are made from the item.
[[maybe_unused]] void boundTemplate_of_control() {
    [[maybe_unused]] DataTemplate const itemTemplate =
        boundTemplate(u"TextBlock", {{u"Text", [](Object const& item) { return item; }}});
}

}  // namespace
