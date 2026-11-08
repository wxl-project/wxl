// Compile-time check of the declarative surface of the list controls.
// Built into the surface test of every profile that selects the component,
// beside test/dsl_surface.cpp; nothing here runs.

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

}  // namespace
