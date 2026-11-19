// Страница SelectorBar -- SelectorBarPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <vector>
#include "Box.h"
#include "IndexList.h"
#include "NavigationPages.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.Animation.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/SelectorBar/BasicSelectorbar.html"};
gallery::LazyLoadedText basicCode{"Snippets/SelectorBar/BasicSelectorbar.h"};

FrameworkElement basic() {
#include "Snippets/SelectorBar/BasicSelectorbar.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText frameTransitionsHeader{"Snippets/SelectorBar/SelectorbarFrameSlideTransitions.html"};
gallery::LazyLoadedText frameTransitionsCode{"Snippets/SelectorBar/SelectorbarFrameSlideTransitions.h"};

FrameworkElement frameTransitions() {
#include "Snippets/SelectorBar/SelectorbarFrameSlideTransitions.h"

    return gallery::controlExample({
        .header = gallery::snippet(frameTransitionsHeader),
        .example = example,
        .code = gallery::snippet(frameTransitionsCode),
    });
}

gallery::LazyLoadedText collectionsHeader{"Snippets/SelectorBar/SelectorbarDisplayingDifferentCollections.html"};
gallery::LazyLoadedText collectionsCode{"Snippets/SelectorBar/SelectorbarDisplayingDifferentCollections.h"};

FrameworkElement collections() {
#include "Snippets/SelectorBar/SelectorbarDisplayingDifferentCollections.h"

    return gallery::controlExample({
        .header = gallery::snippet(collectionsHeader),
        .example = example,
        .code = gallery::snippet(collectionsCode),
    });
}

}  // namespace

static wxl::FrameworkElement selectorBarPage() {
    return StackPanel {basic(), frameTransitions(), collections()};
}

REGISTER_GALLERY_PAGE(L"SelectorBar", selectorBarPage);
