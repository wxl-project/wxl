// Compile-time check of the declarative surface of the custom layouts, with
// the lines of their schema entries (custom_layouts.schema.h). Built into the
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
        [](Collection<UIElement> const& children, Size available) { return available; },
        [](Collection<UIElement> const& children, Size final) { return final; },
    };
    LayoutPanel{layout = custom};
}

// AvailableSizeLayout -- written by hand, so its schema lines are too. The two properties are reported, never taken:
// BindInput is the one form they accept.
[[maybe_unused]] void AvailableSizeLayout_availableWidth_bound(
    ::wxl::AvailableSizeLayout const& object, ::wxl::core::observable<double>& field) {
    ::wxl::impl::apply_argument(
        object, ::wxl::dsl::schema::AvailableSizeLayout::availableWidth = ::wxl::BindInput{field});
    ::wxl::impl::apply_argument(
        object, ::wxl::dsl::schema::AvailableSizeLayout::availableHeight = ::wxl::BindInput{field});
}
[[maybe_unused]] void AvailableSizeLayout_in_braces(::wxl::core::observable<double>& width,
                                                    ::wxl::core::observable<double>& height) {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    LayoutPanel{layout = AvailableSizeLayout{availableWidth = BindInput{width},
                                             availableHeight = BindInput{height}},
                TextBlock{u"page"}};
}

}  // namespace
