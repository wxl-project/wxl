// Страница MenuFlyout -- MenuFlyoutPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText appBarButtonMenuHeader{"Snippets/MenuFlyout/AppBarButtonMenu.html"};
gallery::LazyLoadedText appBarButtonMenuCode{"Snippets/MenuFlyout/AppBarButtonMenu.h"};

FrameworkElement appBarButtonMenu() {
#include "Snippets/MenuFlyout/AppBarButtonMenu.h"

    return gallery::controlExample({
        .header = gallery::snippet(appBarButtonMenuHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(appBarButtonMenuCode),
    });
}

gallery::LazyLoadedText togglesHeader{"Snippets/MenuFlyout/Toggles.html"};
gallery::LazyLoadedText togglesCode{"Snippets/MenuFlyout/Toggles.h"};

FrameworkElement toggles() {
#include "Snippets/MenuFlyout/Toggles.h"

    return gallery::controlExample({
        .header = gallery::snippet(togglesHeader),
        .example = button,
        .code = gallery::snippet(togglesCode),
    });
}

gallery::LazyLoadedText cascadingHeader{"Snippets/MenuFlyout/Cascading.html"};
gallery::LazyLoadedText cascadingCode{"Snippets/MenuFlyout/Cascading.h"};

FrameworkElement cascading() {
#include "Snippets/MenuFlyout/Cascading.h"

    return gallery::controlExample({
        .header = gallery::snippet(cascadingHeader),
        .example = button,
        .code = gallery::snippet(cascadingCode),
    });
}

gallery::LazyLoadedText splitHeader{"Snippets/MenuFlyout/Split.html"};
gallery::LazyLoadedText splitCode{"Snippets/MenuFlyout/Split.h"};

FrameworkElement split() {
#include "Snippets/MenuFlyout/Split.h"

    return gallery::controlExample({
        .header = gallery::snippet(splitHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(splitCode),
    });
}

gallery::LazyLoadedText iconsHeader{"Snippets/MenuFlyout/Icons.html"};
gallery::LazyLoadedText iconsCode{"Snippets/MenuFlyout/Icons.h"};

FrameworkElement icons() {
#include "Snippets/MenuFlyout/Icons.h"

    return gallery::controlExample({
        .header = gallery::snippet(iconsHeader),
        .example = button,
        .code = gallery::snippet(iconsCode),
    });
}

gallery::LazyLoadedText iconsAcceleratorsHeader{"Snippets/MenuFlyout/IconsAccelerators.html"};
gallery::LazyLoadedText iconsAcceleratorsCode{"Snippets/MenuFlyout/IconsAccelerators.h"};

FrameworkElement iconsAccelerators() {
#include "Snippets/MenuFlyout/IconsAccelerators.h"

    return gallery::controlExample({
        .header = gallery::snippet(iconsAcceleratorsHeader),
        .example = button,
        .code = gallery::snippet(iconsAcceleratorsCode),
    });
}

gallery::LazyLoadedText radioHeader{"Snippets/MenuFlyout/Radio.html"};
gallery::LazyLoadedText radioCode{"Snippets/MenuFlyout/Radio.h"};

FrameworkElement radio() {
#include "Snippets/MenuFlyout/Radio.h"

    return gallery::controlExample({
        .header = gallery::snippet(radioHeader),
        .example = button,
        .code = gallery::snippet(radioCode),
    });
}

}  // namespace

static wxl::FrameworkElement menuFlyoutPage() {
    return StackPanel {appBarButtonMenu(), toggles(), cascading(), split(), icons(), iconsAccelerators(), radio()};
}

REGISTER_GALLERY_PAGE(L"MenuFlyout", menuFlyoutPage);
