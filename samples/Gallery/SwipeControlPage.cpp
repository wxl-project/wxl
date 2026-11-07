// Страница SwipeControl -- SwipeControlPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText revealActionsHeader{"Snippets/SwipeControl/RevealActions.html"};
gallery::LazyLoadedText revealActionsCode{"Snippets/SwipeControl/RevealActions.h"};

FrameworkElement revealActions() {
#include "Snippets/SwipeControl/RevealActions.h"

    return gallery::controlExample({
        .header = gallery::snippet(revealActionsHeader),
        .example = swipe,
        .code = gallery::snippet(revealActionsCode),
    });
}

gallery::LazyLoadedText invokeExecuteHeader{"Snippets/SwipeControl/InvokeExecute.html"};
gallery::LazyLoadedText invokeExecuteCode{"Snippets/SwipeControl/InvokeExecute.h"};

FrameworkElement invokeExecute() {
#include "Snippets/SwipeControl/InvokeExecute.h"

    return gallery::controlExample({
        .header = gallery::snippet(invokeExecuteHeader),
        .example = swipe,
        .code = gallery::snippet(invokeExecuteCode),
    });
}

gallery::LazyLoadedText listViewHeader{"Snippets/SwipeControl/ListView.html"};
gallery::LazyLoadedText listViewCode{"Snippets/SwipeControl/ListView.h"};

FrameworkElement listView() {
#include "Snippets/SwipeControl/ListView.h"

    return gallery::controlExample({
        .header = gallery::snippet(listViewHeader),
        .example = list,
        .code = gallery::snippet(listViewCode),
    });
}

gallery::LazyLoadedText gradientHeader{"Snippets/SwipeControl/GradientBackground.html"};
gallery::LazyLoadedText gradientCode{"Snippets/SwipeControl/GradientBackground.h"};

FrameworkElement gradient() {
#include "Snippets/SwipeControl/GradientBackground.h"

    return gallery::controlExample({
        .header = gallery::snippet(gradientHeader),
        .example = swipe,
        .code = gallery::snippet(gradientCode),
    });
}

gallery::LazyLoadedText customIconsHeader{"Snippets/SwipeControl/CustomIcons.html"};
gallery::LazyLoadedText customIconsCode{"Snippets/SwipeControl/CustomIcons.h"};

FrameworkElement customIcons() {
#include "Snippets/SwipeControl/CustomIcons.h"

    return gallery::controlExample({
        .header = gallery::snippet(customIconsHeader),
        .example = swipe,
        .code = gallery::snippet(customIconsCode),
    });
}

}  // namespace

static wxl::FrameworkElement swipeControlPage() {
    return StackPanel {revealActions(), invokeExecute(), listView(), gradient(), customIcons()};
}

REGISTER_GALLERY_PAGE(L"SwipeControl", swipeControlPage);
