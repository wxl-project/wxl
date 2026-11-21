// Страница NavigationView -- NavigationViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <string>
#include <vector>
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include "NavigationPages.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>


using namespace wxl;

namespace {

gallery::LazyLoadedText defaultModeHeader{"Snippets/NavigationView/NavigationviewDefaultPanedisplaymode.html"};
gallery::LazyLoadedText defaultModeCode{"Snippets/NavigationView/NavigationviewDefaultPanedisplaymode.h"};

FrameworkElement defaultMode() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/NavigationviewDefaultPanedisplaymode.h"

    return gallery::controlExample({
        .header = gallery::snippet(defaultModeHeader),
        .example = example,
        .code = gallery::snippet(defaultModeCode),
    });
}

gallery::LazyLoadedText topModeHeader{"Snippets/NavigationView/NavigationviewPanedisplaymodeTop.html"};
gallery::LazyLoadedText topModeCode{"Snippets/NavigationView/NavigationviewPanedisplaymodeTop.h"};

FrameworkElement topMode() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/NavigationviewPanedisplaymodeTop.h"

    return gallery::controlExample({
        .header = gallery::snippet(topModeHeader),
        .example = example,
        .code = gallery::snippet(topModeCode),
    });
}

gallery::LazyLoadedText switchingModeHeader{"Snippets/NavigationView/NavigationviewSwitchesPaneOrientation.html"};
gallery::LazyLoadedText switchingModeCode{"Snippets/NavigationView/NavigationviewSwitchesPaneOrientation.h"};

FrameworkElement switchingMode() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/NavigationviewSwitchesPaneOrientation.h"

    return gallery::controlExample({
        .header = gallery::snippet(switchingModeHeader),
        .example = example,
        .code = gallery::snippet(switchingModeCode),
    });
}

gallery::LazyLoadedText tabsPatternHeader{"Snippets/NavigationView/NavigationViewTyingSelectionFocusTabs.html"};
gallery::LazyLoadedText tabsPatternCode{"Snippets/NavigationView/NavigationViewTyingSelectionFocusTabs.h"};

FrameworkElement tabsPattern() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/NavigationViewTyingSelectionFocusTabs.h"

    return gallery::controlExample({
        .header = gallery::snippet(tabsPatternHeader),
        .example = example,
        .code = gallery::snippet(tabsPatternCode),
    });
}

gallery::LazyLoadedText dataBindingHeader{"Snippets/NavigationView/NavigationViewDataBinding.html"};
gallery::LazyLoadedText dataBindingCode{"Snippets/NavigationView/NavigationViewDataBinding.h"};

FrameworkElement dataBinding() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/NavigationViewDataBinding.h"

    return gallery::controlExample({
        .header = gallery::snippet(dataBindingHeader),
        .example = example,
        .code = gallery::snippet(dataBindingCode),
    });
}

gallery::LazyLoadedText footerItemsHeader{"Snippets/NavigationView/NavigationviewFooterMenuItems.html"};
gallery::LazyLoadedText footerItemsCode{"Snippets/NavigationView/NavigationviewFooterMenuItems.h"};

FrameworkElement footerItems() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/NavigationviewFooterMenuItems.h"

    return gallery::controlExample({
        .header = gallery::snippet(footerItemsHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(footerItemsCode),
    });
}

gallery::LazyLoadedText hierarchicalHeader{"Snippets/NavigationView/HierarchicalNavigationview.html"};
gallery::LazyLoadedText hierarchicalCode{"Snippets/NavigationView/HierarchicalNavigationview.h"};

FrameworkElement hierarchical() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/HierarchicalNavigationview.h"

    return gallery::controlExample({
        .header = gallery::snippet(hierarchicalHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(hierarchicalCode),
    });
}

gallery::LazyLoadedText apiActionHeader{"Snippets/NavigationView/NavigationViewApiAction.html"};
gallery::LazyLoadedText apiActionCode{"Snippets/NavigationView/NavigationViewApiAction.h"};

FrameworkElement apiAction() {
    using namespace wxl::dsl;
#include "Snippets/NavigationView/NavigationViewApiAction.h"

    return gallery::controlExample({
        .header = gallery::snippet(apiActionHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(apiActionCode),
    });
}

}  // namespace

static wxl::FrameworkElement navigationViewPage() {
    return StackPanel {defaultMode(), topMode(), switchingMode(), tabsPattern(), dataBinding(), footerItems(), hierarchical(), apiAction()};
}

REGISTER_GALLERY_PAGE(L"NavigationView", navigationViewPage);
