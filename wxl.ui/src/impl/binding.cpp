// The binding pairs that are functions of their own (binding.h), the winrt
// side: wired so that nothing holds the model and nothing outlives what it
// points at.
//
// The projection and the Windows headers come first, and with them the standard
// library they pull in: the wxl headers below carry the wxl.core import, and a
// standard header after that import is one MSVC has already seen through the std
// module.
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>

#include "../Object.impl.h"
#include <wxl/Members.h>
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>
#include <wxl/Microsoft.UI.Xaml.h>
#include <wxl/Microsoft.UI.Xaml.impl.h>
#include "binding.h"
#include "state_pair.h"

namespace wxl::impl {
namespace {

// What the intermediate-value pair leaves on the box: the Loaded handler that
// finds the inner text box, and the TextChanged handler it puts there. Held
// by the field's watch alone, so both come off when the field goes or
// unbind() runs -- while the handlers still name the field by bare address.
struct intermediate_revoker : core::sta_refcounted {
    NumberBox box;
    EventToken loaded;
    winrt::Microsoft::UI::Xaml::Controls::TextBox input{nullptr};
    winrt::event_token changed;

    explicit intermediate_revoker(NumberBox const& b) : box(b) {}

    ~intermediate_revoker() {
        if (input) input.TextChanged(changed);
        if (loaded) event_adder<EventKey::Loaded>::remove(box, loaded);
    }
};

}  // namespace

// NumberBox commits to Value on Enter, a spin or the focus leaving, and says
// nothing in between; the number as typed is read off the TextBox inside its
// template instead, on every change of its text. The text is parsed by the
// box's own NumberFormatter -- the parser the box will use itself when it
// commits, so the field sees what Value is about to become, in the box's own
// locale. Text that is not a number yet ("-", "1e") parses to nothing, and
// nothing is NaN, the box's own word for an empty field.
//
// The template is applied once the box is in the tree, so the inner box is
// found on Loaded, and once: a box unloaded and loaded again keeps its
// template. The handler holds the parser, not the box -- a handler on the
// inner box holding the outer would be a cycle -- and the field by address,
// which the guard makes safe. The direction is input: the pair's own is, and
// bind_property refuses every other form on it before it gets here.
void apply_bind_intermediate_value(NumberBox const& control, core::observable<double>& model,
                                   bind_direction) {
    using Args = typename event_adder<EventKey::Loaded>::template args_t<NumberBox>;
    using winrt::Windows::Globalization::NumberFormatting::INumberParser;
    namespace xaml = winrt::Microsoft::UI::Xaml;

    core::intrusive_ptr<intermediate_revoker> revoker{new intermediate_revoker{control},
                                                      /*add_ref=*/false};
    intermediate_revoker* const state = revoker.get();

    state->loaded = event_adder<EventKey::Loaded>::add(
        control, [&model, state](NumberBox const& sender, Args&) {
            if (state->input) return;

            // GetTemplateChild is protected, and the projection puts it on the
            // protected interface rather than on the class.
            xaml::Controls::NumberBox const& box = *Object::Impl::get_typed<NumberBox>(sender);
            auto const input = box.as<xaml::Controls::IControlProtected>()
                                   .GetTemplateChild(L"InputBox")
                                   .try_as<xaml::Controls::TextBox>();
            if (!input) return;
            INumberParser const parser = box.NumberFormatter().as<INumberParser>();

            state->input = input;
            state->changed = input.TextChanged(
                [&model, parser](winrt::Windows::Foundation::IInspectable const& sender,
                                 xaml::Controls::TextChangedEventArgs const&) {
                    auto const number =
                        parser.ParseDouble(sender.as<xaml::Controls::TextBox>().Text());
                    model.set(number ? number.Value() : std::numeric_limits<double>::quiet_NaN());
                });
        });

    model.watch_for_binding([revoker = std::move(revoker)](double const&) noexcept {});
}

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

// A viewer as settle_offset sees it. The jump is ChangeView without animation:
// the field is not left between an offset asked and a scroll still under way.
struct offset_viewer {
    xaml::Controls::ScrollViewer viewer;
    xaml::FrameworkElement framework;

    bool loaded() const { return framework.IsLoaded(); }
    double offset() const { return viewer.VerticalOffset(); }
    double extent() const { return viewer.ScrollableHeight(); }
    bool scroll_to(double offset) const {
        return viewer.ChangeView(nullptr, winrt::Windows::Foundation::IReference<double>{offset},
                                 nullptr, true);
    }
};

// What a state pair leaves on its control: the handlers reporting into the
// field, and, both ways, the Loaded handler that serves a request made while the
// control was out of the tree. Held by the field's watch alone, as
// intermediate_revoker is: the handlers name it and the field by bare address,
// and come off before either is gone.
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

struct offset_revoker : core::sta_refcounted {
    offset_viewer target;
    core::observable<double>& field;
    winrt::event_token changed;
    winrt::event_token loaded;

    offset_revoker(xaml::Controls::ScrollViewer const& viewer, core::observable<double>& f)
        : target{viewer, viewer.as<xaml::FrameworkElement>()}, field(f) {}

    ~offset_revoker() {
        target.viewer.ViewChanged(changed);
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

// The offset is reported once the view has settled; the steps of a pan or of an
// animation in between are not offsets the field could be asked to go back to.
// The watch is in place before the first settle, which may correct the field and
// relies on the watch to act on the correction.
void apply_bind_vertical_offset(ScrollViewer const& control, core::observable<double>& model,
                                bind_direction direction) {
    xaml::Controls::ScrollViewer const& viewer = *Object::Impl::get_typed<ScrollViewer>(control);
    core::intrusive_ptr<offset_revoker> revoker{new offset_revoker{viewer, model},
                                                /*add_ref=*/false};
    offset_revoker* const state = revoker.get();

    state->changed =
        viewer.ViewChanged([state](winrt::Windows::Foundation::IInspectable const&,
                                   xaml::Controls::ScrollViewerViewChangedEventArgs const& args) {
            if (!args.IsIntermediate()) state->field.set(state->target.offset());
        });

    if (direction == bind_direction::input) {
        model.set(state->target.offset());
        model.watch_for_binding([revoker = std::move(revoker)](double const&) noexcept {});
        return;
    }

    state->loaded = state->target.framework.Loaded(
        [state](winrt::Windows::Foundation::IInspectable const&, xaml::RoutedEventArgs const&) {
            settle_offset(state->target, state->field);
        });
    model.watch_for_binding([revoker = std::move(revoker)](double const&) noexcept {
        settle_offset(revoker->target, revoker->field);
    });
    settle_offset(state->target, model);
}

}  // namespace wxl::impl
