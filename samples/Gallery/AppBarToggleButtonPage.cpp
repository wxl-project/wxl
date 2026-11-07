// Страница AppBarToggleButton -- AppBarToggleButtonPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText symbolIconHeader{"Snippets/AppBarToggleButton/SymbolIcon.html"};
gallery::LazyLoadedText symbolIconCode{"Snippets/AppBarToggleButton/SymbolIcon.h"};

FrameworkElement symbolIcon() {
#include "Snippets/AppBarToggleButton/SymbolIcon.h"

    return gallery::controlExample({
        .header = gallery::snippet(symbolIconHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(symbolIconCode),
    });
}

gallery::LazyLoadedText bitmapIconHeader{"Snippets/AppBarToggleButton/BitmapIcon.html"};
gallery::LazyLoadedText bitmapIconCode{"Snippets/AppBarToggleButton/BitmapIcon.h"};

FrameworkElement bitmapIcon() {
#include "Snippets/AppBarToggleButton/BitmapIcon.h"

    return gallery::controlExample({
        .header = gallery::snippet(bitmapIconHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(bitmapIconCode),
    });
}

gallery::LazyLoadedText fontIconHeader{"Snippets/AppBarToggleButton/FontIcon.html"};
gallery::LazyLoadedText fontIconCode{"Snippets/AppBarToggleButton/FontIcon.h"};

FrameworkElement fontIcon() {
#include "Snippets/AppBarToggleButton/FontIcon.h"

    return gallery::controlExample({
        .header = gallery::snippet(fontIconHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(fontIconCode),
    });
}

gallery::LazyLoadedText threeStateHeader{"Snippets/AppBarToggleButton/ThreeState.html"};
gallery::LazyLoadedText threeStateCode{"Snippets/AppBarToggleButton/ThreeState.h"};

FrameworkElement threeState() {
#include "Snippets/AppBarToggleButton/ThreeState.h"

    return gallery::controlExample({
        .header = gallery::snippet(threeStateHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .code = gallery::snippet(threeStateCode),
    });
}

}  // namespace

static wxl::FrameworkElement appBarToggleButtonPage() {
    return StackPanel {symbolIcon(), bitmapIcon(), fontIcon(), threeState()};
}

REGISTER_GALLERY_PAGE(L"AppBarToggleButton", appBarToggleButtonPage);
