// Compile-time check of the declarative surface of the dialogs.
// Built into the surface test of every profile that selects the component,
// beside test/dsl_surface.cpp; nothing here runs.

#include "ShowDialog.h"
#include "ui.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// Tabs at the bottom of a window and the dialog that answers by events: both
// are here so that a change to the profile or to showDialog's signature stops
// this translation unit rather than an application.
[[maybe_unused]] void tabs_and_dialogs(Window const& window, async::cancellation_token token) {
    SelectorBar tabs{
        hAlign.center,
        onSelectionChanged += [](Object const&, SelectorBarSelectionChangedEventArgs&) {},
        SelectorBarItem{L"Messages", icon = FontIcon{glyph = L""}},
        SelectorBarItem{L"Watched", icon = FontIcon{glyph = L""}},
    };

    tabs.selectedItem(tabs.items()[0]);

    ContentDialog about{
        closeButtonText = L"Close",
        content = TextBlock{L"Nothing to say yet"},
    };

    // The wait for the answer is a task its owner keeps; here, until the function returns.
    async::task<> const overWindow = showDialog(about, window, token);
    async::task<> const overContent = showDialog(about, window.content(), token);
}

// showDialog takes the token its owner closes the dialog with, and has no form without one: the
// wait for the answer is a body that has to be over before the exit, and a dialog nobody can close
// would keep it alive there.
template <class Host>
concept shows_dialog_without_a_token =
    requires(ContentDialog const& dialog, Host const& host) { showDialog(dialog, host); };

static_assert(!shows_dialog_without_a_token<Window> && !shows_dialog_without_a_token<UIElement>);

}  // namespace
