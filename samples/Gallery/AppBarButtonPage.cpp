// Страница AppBarButton -- AppBarButtonPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText symbolIconHeader{"Snippets/AppBarButton/SymbolIcon.html"};
gallery::LazyLoadedText symbolIconCode{"Snippets/AppBarButton/SymbolIcon.h"};

FrameworkElement symbolIcon() {
#include "Snippets/AppBarButton/SymbolIcon.h"

    return gallery::controlExample({
        .header = gallery::snippet(symbolIconHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(symbolIconCode),
    });
}

gallery::LazyLoadedText bitmapIconHeader{"Snippets/AppBarButton/BitmapIcon.html"};
gallery::LazyLoadedText bitmapIconCode{"Snippets/AppBarButton/BitmapIcon.h"};

FrameworkElement bitmapIcon() {
#include "Snippets/AppBarButton/BitmapIcon.h"

    return gallery::controlExample({
        .header = gallery::snippet(bitmapIconHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(bitmapIconCode),
    });
}

gallery::LazyLoadedText fontIconHeader{"Snippets/AppBarButton/FontIcon.html"};
gallery::LazyLoadedText fontIconCode{"Snippets/AppBarButton/FontIcon.h"};

FrameworkElement fontIcon() {
#include "Snippets/AppBarButton/FontIcon.h"

    return gallery::controlExample({
        .header = gallery::snippet(fontIconHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(fontIconCode),
    });
}

gallery::LazyLoadedText pathIconHeader{"Snippets/AppBarButton/PathIcon.html"};
gallery::LazyLoadedText pathIconCode{"Snippets/AppBarButton/PathIcon.h"};

FrameworkElement pathIcon() {
#include "Snippets/AppBarButton/PathIcon.h"

    return gallery::controlExample({
        .header = gallery::snippet(pathIconHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(pathIconCode),
    });
}

gallery::LazyLoadedText acceleratorHeader{"Snippets/AppBarButton/KeyboardAccelerator.html"};
gallery::LazyLoadedText acceleratorCode{"Snippets/AppBarButton/KeyboardAccelerator.h"};

FrameworkElement accelerator() {
#include "Snippets/AppBarButton/KeyboardAccelerator.h"

    return gallery::controlExample({
        .header = gallery::snippet(acceleratorHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(acceleratorCode),
    });
}

gallery::LazyLoadedText flyoutButtonHeader{"Snippets/AppBarButton/Flyout.html"};
gallery::LazyLoadedText flyoutButtonCode{"Snippets/AppBarButton/Flyout.h"};

FrameworkElement flyoutButton() {
#include "Snippets/AppBarButton/Flyout.h"

    return gallery::controlExample({
        .header = gallery::snippet(flyoutButtonHeader),
        .example = StackPanel {orientation.horizontal, button},
        .code = gallery::snippet(flyoutButtonCode),
    });
}

}  // namespace

static wxl::FrameworkElement appBarButtonPage() {
    return StackPanel {symbolIcon(), bitmapIcon(), fontIcon(), pathIcon(), accelerator(), flyoutButton()};
}

REGISTER_GALLERY_PAGE(L"AppBarButton", appBarButtonPage);
