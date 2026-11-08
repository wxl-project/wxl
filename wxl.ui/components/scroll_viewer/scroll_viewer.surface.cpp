// Compile-time check of the declarative surface of the scroll viewer.
// Built into the surface test of every profile that selects the component,
// beside test/dsl_surface.cpp; nothing here runs.

#include "ui.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// A ScrollViewer that hands the wheel it cannot use to the scroller above it.
[[maybe_unused]] void wheel_to_parent() {
    ScrollViewer{wheelToParent = true, horizontalScrollBarVisibility = ScrollBarVisibility::Auto};
}

}  // namespace
