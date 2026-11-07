// Страница TabView -- TabViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <algorithm>
#include <string>
#include <vector>
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include "NavigationPages.h"
#include "RepeaterData.h"
#include "Resource.h"
#include "TabViewWindowing.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Input.h>
#include <wxl/Microsoft.UI.Xaml.Media.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText addingClosingHeader{"Snippets/TabView/TabviewSupportAddingClosing.html"};
gallery::LazyLoadedText addingClosingCode{"Snippets/TabView/TabviewSupportAddingClosing.h"};

FrameworkElement addingClosing() {
#include "Snippets/TabView/TabviewSupportAddingClosing.h"

    return gallery::controlExample({
        .header = gallery::snippet(addingClosingHeader),
        .example = example,
        .code = gallery::snippet(addingClosingCode),
    });
}

gallery::LazyLoadedText itemsInMarkupHeader{"Snippets/TabView/TabviewTabviewitemsDefinedMarkup.html"};
gallery::LazyLoadedText itemsInMarkupCode{"Snippets/TabView/TabviewTabviewitemsDefinedMarkup.h"};

FrameworkElement itemsInMarkup() {
#include "Snippets/TabView/TabviewTabviewitemsDefinedMarkup.h"

    return gallery::controlExample({
        .header = gallery::snippet(itemsInMarkupHeader),
        .example = example,
        .code = gallery::snippet(itemsInMarkupCode),
    });
}

gallery::LazyLoadedText boundCollectionHeader{"Snippets/TabView/TabviewBoundCollectionMydata.html"};
gallery::LazyLoadedText boundCollectionCode{"Snippets/TabView/TabviewBoundCollectionMydata.h"};

FrameworkElement boundCollection() {
#include "Snippets/TabView/TabviewBoundCollectionMydata.h"

    return gallery::controlExample({
        .header = gallery::snippet(boundCollectionHeader),
        .example = example,
        .code = gallery::snippet(boundCollectionCode),
    });
}

gallery::LazyLoadedText keyboardingHeader{"Snippets/TabView/TabviewKeyboardingSupport.html"};
gallery::LazyLoadedText keyboardingCode{"Snippets/TabView/TabviewKeyboardingSupport.h"};

FrameworkElement keyboarding() {
#include "Snippets/TabView/TabviewKeyboardingSupport.h"

    return gallery::controlExample({
        .header = gallery::snippet(keyboardingHeader),
        .example = example,
        .code = gallery::snippet(keyboardingCode),
    });
}

gallery::LazyLoadedText customContentHeader{"Snippets/TabView/TabViewYouPutCustomContent.html"};
gallery::LazyLoadedText customContentCode{"Snippets/TabView/TabViewYouPutCustomContent.h"};

FrameworkElement customContent() {
#include "Snippets/TabView/TabViewYouPutCustomContent.h"

    return gallery::controlExample({
        .header = gallery::snippet(customContentHeader),
        .example = example,
        .code = gallery::snippet(customContentCode),
    });
}

gallery::LazyLoadedText tabWidthsHeader{"Snippets/TabView/TabViewTabWidthsEitherBe.html"};
gallery::LazyLoadedText tabWidthsCode{"Snippets/TabView/TabViewTabWidthsEitherBe.h"};

FrameworkElement tabWidths() {
#include "Snippets/TabView/TabViewTabWidthsEitherBe.h"

    return gallery::controlExample({
        .header = gallery::snippet(tabWidthsHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(tabWidthsCode),
    });
}

gallery::LazyLoadedText closeButtonHeader{"Snippets/TabView/TabViewCloseButtonBePersistent.html"};
gallery::LazyLoadedText closeButtonCode{"Snippets/TabView/TabViewCloseButtonBePersistent.h"};

FrameworkElement closeButton() {
#include "Snippets/TabView/TabViewCloseButtonBePersistent.h"

    return gallery::controlExample({
        .header = gallery::snippet(closeButtonHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(closeButtonCode),
    });
}

gallery::LazyLoadedText colorIconsHeader{"Snippets/TabView/TabviewColorTabIcons.html"};
gallery::LazyLoadedText colorIconsCode{"Snippets/TabView/TabviewColorTabIcons.h"};

FrameworkElement colorIcons() {
#include "Snippets/TabView/TabviewColorTabIcons.h"

    return gallery::controlExample({
        .header = gallery::snippet(colorIconsHeader),
        .example = example,
        .code = gallery::snippet(colorIconsCode),
    });
}

gallery::LazyLoadedText accentStripHeader{"Snippets/TabView/TabviewAccentColoredTabstrip.html"};
gallery::LazyLoadedText accentStripCode{"Snippets/TabView/TabviewAccentColoredTabstrip.h"};

FrameworkElement accentStrip() {
#include "Snippets/TabView/TabviewAccentColoredTabstrip.h"

    return gallery::controlExample({
        .header = gallery::snippet(accentStripHeader),
        .example = example,
        .code = gallery::snippet(accentStripCode),
    });
}

gallery::LazyLoadedText windowingHeader{"Snippets/TabView/CompleteTabviewWindowingSample.html"};
gallery::LazyLoadedText windowingCode{"Snippets/TabView/CompleteTabviewWindowingSample.h"};

FrameworkElement windowing() {
#include "Snippets/TabView/CompleteTabviewWindowingSample.h"

    return gallery::controlExample({
        .header = gallery::snippet(windowingHeader),
        .example = example,
        .code = gallery::snippet(windowingCode),
    });
}

}  // namespace

static wxl::FrameworkElement tabViewPage() {
    return StackPanel {addingClosing(), itemsInMarkup(), boundCollection(), keyboarding(), customContent(), tabWidths(), closeButton(), colorIcons(), accentStrip(), windowing()};
}

REGISTER_GALLERY_PAGE(L"TabView", tabViewPage);
