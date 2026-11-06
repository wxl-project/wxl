#pragma once

// The binding pair of UIElement.isFocused, a function of its own (binding.h): the type map names
// it ("boundMembers"), and the generated pair calls it. Declared without winrt, as the generated
// pairs are; defined in focus_pair.cpp, where the projection is allowed.

#include "../core.h"

#include "member.h"

namespace wxl {
class UIElement;
}

namespace wxl::impl {

/// UIElement.isFocused <-> observable<bool>, under GotFocus and LostFocus: whether
/// the element itself has the focus. True asks for it -- an element out of the
/// tree is asked when it is loaded -- and the field then says whether the
/// element took it; false is no request, since XAML gives focus up only by
/// giving it to another element, so a focused element reads true again. A
/// control that hands its focus to a part of its template reads false.
void apply_bind_focus(UIElement const& control, core::observable<bool>& model,
                      bind_direction direction);

}  // namespace wxl::impl
