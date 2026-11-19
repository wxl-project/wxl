// Страница Flyout -- FlyoutPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText buttonHeader{"Snippets/Flyout/ButtonFlyout.html"};
gallery::LazyLoadedText buttonCode{"Snippets/Flyout/ButtonFlyout.h"};

FrameworkElement button() {
#include "Snippets/Flyout/ButtonFlyout.h"

    return gallery::controlExample({
        .header = gallery::snippet(buttonHeader),
        .example = example,
        .code = gallery::snippet(buttonCode),
    });
}

}  // namespace

static wxl::FrameworkElement flyoutPage() {
    return StackPanel {button()};
}

REGISTER_GALLERY_PAGE(L"Flyout", flyoutPage);
