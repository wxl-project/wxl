// Страница ThemeTransition -- ThemeTransitionPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <memory>
#include <string>
#include <vector>
#include "Box.h"
#include "RepeaterData.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.Animation.h>
#include <wxl/Microsoft.UI.Xaml.Shapes.h>
#include <wxl/Microsoft.UI.Xaml.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText entranceExampleHeader{"Snippets/ThemeTransition/EntrancethemetransitionAddingItemsYour.html"};
gallery::LazyLoadedText entranceExampleCode{"Snippets/ThemeTransition/EntrancethemetransitionAddingItemsYour.h"};

FrameworkElement entranceExample() {
#include "Snippets/ThemeTransition/EntrancethemetransitionAddingItemsYour.h"

    return gallery::controlExample({
        .header = gallery::snippet(entranceExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(entranceExampleCode),
    });
}

gallery::LazyLoadedText repositionExampleHeader{"Snippets/ThemeTransition/RepositionthemetransitionReactLayoutChanges.html"};
gallery::LazyLoadedText repositionExampleCode{"Snippets/ThemeTransition/RepositionthemetransitionReactLayoutChanges.h"};

FrameworkElement repositionExample() {
#include "Snippets/ThemeTransition/RepositionthemetransitionReactLayoutChanges.h"

    return gallery::controlExample({
        .header = gallery::snippet(repositionExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(repositionExampleCode),
    });
}

gallery::LazyLoadedText contentExampleHeader{"Snippets/ThemeTransition/ContentthemetransitionAnimateContentRefreshes.html"};
gallery::LazyLoadedText contentExampleCode{"Snippets/ThemeTransition/ContentthemetransitionAnimateContentRefreshes.h"};

FrameworkElement contentExample() {
#include "Snippets/ThemeTransition/ContentthemetransitionAnimateContentRefreshes.h"

    return gallery::controlExample({
        .header = gallery::snippet(contentExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(contentExampleCode),
    });
}

gallery::LazyLoadedText addDeleteExampleHeader{"Snippets/ThemeTransition/AdddeletethemetransitionAnimateAddingRemoving.html"};
gallery::LazyLoadedText addDeleteExampleCode{"Snippets/ThemeTransition/AdddeletethemetransitionAnimateAddingRemoving.h"};

FrameworkElement addDeleteExample() {
#include "Snippets/ThemeTransition/AdddeletethemetransitionAnimateAddingRemoving.h"

    return gallery::controlExample({
        .header = gallery::snippet(addDeleteExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(addDeleteExampleCode),
    });
}

gallery::LazyLoadedText popupExampleHeader{"Snippets/ThemeTransition/PopupthemetransitionAnimateOpeningClosing.html"};
gallery::LazyLoadedText popupExampleCode{"Snippets/ThemeTransition/PopupthemetransitionAnimateOpeningClosing.h"};

FrameworkElement popupExample() {
#include "Snippets/ThemeTransition/PopupthemetransitionAnimateOpeningClosing.h"

    return gallery::controlExample({
        .header = gallery::snippet(popupExampleHeader),
        .example = example,
        .code = gallery::snippet(popupExampleCode),
    });
}

}  // namespace

static wxl::FrameworkElement themeTransitionPage() {
    return StackPanel {entranceExample(), repositionExample(), contentExample(), addDeleteExample(), popupExample()};
}

REGISTER_GALLERY_PAGE(L"ThemeTransition", themeTransitionPage);
