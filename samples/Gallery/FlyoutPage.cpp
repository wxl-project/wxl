// Страница Flyout -- FlyoutPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t buttonHeader[] = {
#include "Snippets/Flyout/ButtonFlyout.html.embed"
};
constexpr char8_t buttonCode[] = {
#include "Snippets/Flyout/ButtonFlyout.h.embed"
};

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
