// The verticalOffset pair, the winrt side: wired so that nothing holds the
// model and nothing outlives what it points at.
//
// The projection comes first, and with it the standard library it pulls in:
// the wxl headers below carry the wxl.core import, and a standard header after
// that import is one MSVC has already seen through the std module.
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>

#include "../Object.impl.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>
#include "state_pair.h"
#include "vertical_offset_pair.h"

namespace wxl::impl {
namespace {

namespace xaml = winrt::Microsoft::UI::Xaml;

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

// What the pair leaves on its viewer: the handler reporting into the field,
// and, both ways, the Loaded handler that serves a request made while the
// viewer was out of the tree. Held by the field's watch alone: the handlers
// name it and the field by bare address, and come off before either is gone.
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
