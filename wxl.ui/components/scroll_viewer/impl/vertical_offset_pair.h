#pragma once

// The binding pair of ScrollViewer.verticalOffset, a function of its own (binding.h): the type
// map names it ("boundMembers"), and the generated pair calls it. Declared without winrt, as the
// generated pairs are; defined in vertical_offset_pair.cpp, where the projection is allowed.

#include "core.h"

#include "impl/member.h"

namespace wxl {
class ScrollViewer;
}

namespace wxl::impl {

/// ScrollViewer.verticalOffset <-> observable<double>, under ViewChanged once the
/// view has settled. A value scrolls the viewer there without animation, clamped
/// to the content's extent; a viewer out of the tree is scrolled when it is
/// loaded. The field says where the viewer is, or is going: the offset it was
/// clamped to, or the one it stays at when the viewer refuses.
void apply_bind_vertical_offset(ScrollViewer const& control, core::observable<double>& model,
                                bind_direction direction);

}  // namespace wxl::impl
