#pragma once

// The binding pair of NumberBox.intermediateValue, a function of its own (binding.h): the type
// map names it ("boundMembers"), and the generated pair calls it. Declared without winrt, as the
// generated pairs are; defined in intermediate_value_pair.cpp, where the projection is allowed.

#include "core.h"

#include "impl/member.h"

namespace wxl {
class NumberBox;
}

namespace wxl::impl {

/// NumberBox.intermediateValue -> observable<double>: the number as it is
/// being typed, parsed by the box's own NumberFormatter on every change of
/// its text, NaN while the text is not a number yet. Input alone -- the box
/// commits to Value on its own terms, and this is the value before that --
/// so the direction is always input.
void apply_bind_intermediate_value(NumberBox const& control, core::observable<double>& model,
                                   bind_direction direction);

}  // namespace wxl::impl
