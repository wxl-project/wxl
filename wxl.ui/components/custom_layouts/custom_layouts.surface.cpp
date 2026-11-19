// Compile-time check of the declarative surface of the custom layouts where it
// does not follow from their descriptions (component.json). Built into the
// surface test of every profile that selects the component; nothing here runs.

#include "Bind.h"
#include "CustomLayout.h"
#include "ui.h"
#include "schema.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// A layout written as two functions, in a LayoutPanel.
[[maybe_unused]] void custom_layout() {
    auto const custom = CustomLayout{
        [](Collection<UIElement> const& /*elements*/, Size available) { return available; },
        [](Collection<UIElement> const& /*elements*/, Size final) { return final; },
    };
    LayoutPanel{layout = custom};
}

// The two properties are reported, never taken: BindInput is the one form they accept.
[[maybe_unused]] void AvailableSizeLayout_in_braces(::wxl::core::observable<double>& w,
                                                    ::wxl::core::observable<double>& h) {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    LayoutPanel{layout = AvailableSizeLayout{availableWidth = BindInput{w},
                                             availableHeight = BindInput{h}},
                TextBlock{u"page"}};
}

}  // namespace
