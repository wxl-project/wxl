// The projection comes first, and with it every standard header it needs:
// wxl's own headers carry the wxl.core import, and a standard header included
// after that import is one the compiler has already seen through the std
// module.
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>

#include "scroll_controller_helpers.h"

#include "../Object.impl.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>

// Imports last, after every plain header.
import wxl.core;

namespace wxl::impl {

namespace {

namespace controls = winrt::Microsoft::UI::Xaml::Controls;

controls::AnnotatedScrollBar native(AnnotatedScrollBar const& bar) {
    return *Object::Impl::get_typed<AnnotatedScrollBar>(bar);
}

// A ScrollView has its ScrollPresenter once its template is applied, which is
// when it is loaded: before that the presenter is null and there is nothing to
// give a controller to. So a view not yet loaded is given the controller at
// its Loaded, once -- the handler takes itself off, and with it lets go of the
// bar.
template <typename Give>
void give_when_loaded(controls::ScrollView const& view, controls::AnnotatedScrollBar const& bar,
                      Give give) {
    if (auto const presenter = view.ScrollPresenter()) {
        give(presenter, bar);
        return;
    }
    auto const token = core::make_refcounted<winrt::event_token>();
    *token = view.Loaded([bar, give, token](winrt::Windows::Foundation::IInspectable const& sender,
                                            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&) {
        auto const loaded = sender.as<controls::ScrollView>();
        give(loaded.ScrollPresenter(), bar);
        loaded.Loaded(*token);
    });
}

}  // namespace

void set_vertical_scroll_controller(controls::ScrollView const& view,
                                    AnnotatedScrollBar const& bar) {
    give_when_loaded(view, native(bar), [](auto const& presenter, auto const& scrollBar) {
        presenter.VerticalScrollController(scrollBar.ScrollController());
    });
}

void set_horizontal_scroll_controller(controls::ScrollView const& view,
                                      AnnotatedScrollBar const& bar) {
    give_when_loaded(view, native(bar), [](auto const& presenter, auto const& scrollBar) {
        presenter.HorizontalScrollController(scrollBar.ScrollController());
    });
}

}  // namespace wxl::impl
