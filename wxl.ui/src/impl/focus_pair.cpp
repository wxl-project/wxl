// The isFocused pair, the winrt side: wired so that nothing holds the model
// and nothing outlives what it points at.
//
// The projection comes first, and with it the standard library it pulls in:
// the wxl headers below carry the wxl.core import, and a standard header after
// that import is one MSVC has already seen through the std module.
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>

#include "../Object.impl.h"
#include <wxl/Microsoft.UI.Xaml.h>
#include <wxl/Microsoft.UI.Xaml.impl.h>
#include "focus_pair.h"
#include "state_pair.h"

namespace wxl::impl {
namespace {

namespace xaml = winrt::Microsoft::UI::Xaml;

// An element as settle_focus sees it. Loaded and IsLoaded are FrameworkElement's,
// so that face is asked for once; an element that is not one is taken as being
// in the tree, and tries at once.
struct focus_element {
    xaml::UIElement element;
    xaml::FrameworkElement framework;

    bool loaded() const { return !framework || framework.IsLoaded(); }
    bool focused() const { return element.FocusState() != xaml::FocusState::Unfocused; }
    void focus() const { element.Focus(xaml::FocusState::Programmatic); }
};

// What the pair leaves on its element: the handlers reporting into the field,
// and, both ways, the Loaded handler that serves a request made while the
// element was out of the tree. Held by the field's watch alone: the handlers
// name it and the field by bare address, and come off before either is gone.
struct focus_revoker : core::sta_refcounted {
    focus_element target;
    core::observable<bool>& field;
    winrt::event_token got;
    winrt::event_token lost;
    winrt::event_token loaded;

    focus_revoker(xaml::UIElement const& element, core::observable<bool>& f)
        : target{element, element.try_as<xaml::FrameworkElement>()}, field(f) {}

    ~focus_revoker() {
        target.element.GotFocus(got);
        target.element.LostFocus(lost);
        if (loaded) target.framework.Loaded(loaded);
    }
};

}  // namespace

// The focus is reported under GotFocus and LostFocus, and both read the element's
// own FocusState rather than trust the event: the two bubble, so a child's focus
// reaches the parent's handlers too, and they are raised after the fact, when the
// focus may have moved on again.
void apply_bind_focus(UIElement const& control, core::observable<bool>& model,
                      bind_direction direction) {
    xaml::UIElement const& element = *Object::Impl::get_typed<UIElement>(control);
    core::intrusive_ptr<focus_revoker> revoker{new focus_revoker{element, model},
                                               /*add_ref=*/false};
    focus_revoker* const state = revoker.get();

    auto const report = [state](winrt::Windows::Foundation::IInspectable const&,
                                xaml::RoutedEventArgs const&) {
        state->field.set(state->target.focused());
    };
    state->got = element.GotFocus(report);
    state->lost = element.LostFocus(report);

    if (direction == bind_direction::input) {
        model.set(state->target.focused());
        model.watch_for_binding([revoker = std::move(revoker)](bool) noexcept {});
        return;
    }

    if (state->target.framework) {
        state->loaded = state->target.framework.Loaded(
            [state](winrt::Windows::Foundation::IInspectable const&, xaml::RoutedEventArgs const&) {
                settle_focus(state->target, state->field);
            });
    }
    model.watch_for_binding([revoker = std::move(revoker)](bool) noexcept {
        settle_focus(revoker->target, revoker->field);
    });
    settle_focus(state->target, model);
}

}  // namespace wxl::impl
