#pragma once

// `verticalScrollController = annotatedBar` on a ScrollView. The view scrolls through a
// ScrollPresenter, which takes a scroll controller for each direction; an AnnotatedScrollBar is
// one, through its ScrollController property, an interface no wrapper stands for. wxl writes the
// link as a property of the view, holding the bar, and makes it here.
//
// Private: the view arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace wxl {
class AnnotatedScrollBar;
}  // namespace wxl

namespace wxl::impl {

void set_vertical_scroll_controller(winrt::Microsoft::UI::Xaml::Controls::ScrollView const& view,
                                    AnnotatedScrollBar const& bar);

void set_horizontal_scroll_controller(winrt::Microsoft::UI::Xaml::Controls::ScrollView const& view,
                                      AnnotatedScrollBar const& bar);

}  // namespace wxl::impl
