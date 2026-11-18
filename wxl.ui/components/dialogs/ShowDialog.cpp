// showDialog, the winrt side: the XamlRoot, then the dialog shown under the caller's token and
// the wait for its answer handed back (impl/show_dialog.h).
//
// The projection headers come first, and with them the standard library they
// pull in: the wxl headers below carry the wxl.core import, and a standard
// header after that import is one MSVC has already seen through the std
// module.
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>

#include "Object.impl.h"
#include "ShowDialog.h"
#include "impl/show_dialog.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>
#include <wxl/Microsoft.UI.Xaml.h>
#include <wxl/Microsoft.UI.Xaml.impl.h>

namespace wxl {

namespace {

namespace xaml = winrt::Microsoft::UI::Xaml;

// The theme of what the dialog is raised over, for the time it is up. A dialog is not in the
// visual tree: it hangs in the island's popup layer, which inherits nothing from the content or
// from the root of the island, so over a window asked for Dark it would show the application's
// Light. A dialog that asks for a theme of its own keeps it. The borrowed one is handed back when
// the dialog closes -- Default again, so the next show, over a window whose theme has changed
// since, borrows afresh rather than mistake the old one for the dialog's own. The handler takes
// itself off and holds nothing: the dialog arrives as the sender.
//
// Only for a dialog that will close: under a token cancelled already it is not shown, and a
// dialog the platform refuses never comes up -- neither raises Closed, so the first is not lent
// the theme at all, and the second is handed it back at once, the handler taken off.
async::task<> show_in_theme_of(xaml::Controls::ContentDialog const& native,
                               ContentDialog const& dialog, xaml::UIElement const& host,
                               async::stop_token token) {
    auto const shown = host.try_as<xaml::FrameworkElement>();
    if (!shown || native.RequestedTheme() != xaml::ElementTheme::Default || token.stop_requested())
        return impl::show_dialog(dialog, std::move(token));

    native.RequestedTheme(shown.ActualTheme());
    auto const closed = std::make_shared<winrt::event_token>();
    *closed = native.Closed([closed](xaml::Controls::ContentDialog const& sender,
                                     xaml::Controls::ContentDialogClosedEventArgs const&) {
        sender.Closed(*closed);
        sender.RequestedTheme(xaml::ElementTheme::Default);
    });
    try {
        return impl::show_dialog(dialog, std::move(token));
    } catch (...) {
        native.Closed(*closed);
        native.RequestedTheme(xaml::ElementTheme::Default);
        throw;
    }
}

}  // namespace

async::task<> showDialog(ContentDialog const& dialog, UIElement const& host,
                         async::stop_token token) {
    xaml::Controls::ContentDialog const& native = *Object::Impl::get_typed<ContentDialog>(dialog);
    if (!host) return impl::show_dialog(dialog, std::move(token));

    // Which island the dialog hangs over -- the one `host` is already in -- and in which theme.
    if (XamlRoot const root = host.xamlRoot())
        native.XamlRoot(*Object::Impl::get_typed<XamlRoot>(root));
    return show_in_theme_of(native, dialog, *Object::Impl::get_typed<UIElement>(host),
                            std::move(token));
}

// A window is named by what it shows: a dialog raised from a window belongs
// over that same content, in that same island.
async::task<> showDialog(ContentDialog const& dialog, Window const& host,
                         async::stop_token token) {
    return showDialog(dialog, host.content(), std::move(token));
}

}  // namespace wxl
