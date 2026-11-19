// Страница ItemsView -- ItemsViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include "CustomDataObject.h"
#include <wxl/Microsoft.UI.Dispatching.h>
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicViewHeader{"Snippets/ItemsView/BasicItemsview.html"};
gallery::LazyLoadedText basicViewCode{"Snippets/ItemsView/BasicItemsview.h"};

FrameworkElement basicView() {
#include "Snippets/ItemsView/BasicItemsview.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicViewHeader),
        .example = example,
        .code = gallery::snippet(basicViewCode),
    });
}

gallery::LazyLoadedText swappableHeader{"Snippets/ItemsView/ItemsviewSwappableLayouts.html"};
gallery::LazyLoadedText swappableCode{"Snippets/ItemsView/ItemsviewSwappableLayouts.h"};

FrameworkElement swappable() {
#include "Snippets/ItemsView/ItemsviewSwappableLayouts.h"

    return gallery::controlExample({
        .header = gallery::snippet(swappableHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(swappableCode),
    });
}

gallery::LazyLoadedText invocationHeader{"Snippets/ItemsView/ItemsviewItemInvocationSelection.html"};
gallery::LazyLoadedText invocationCode{"Snippets/ItemsView/ItemsviewItemInvocationSelection.h"};

FrameworkElement invocation() {
#include "Snippets/ItemsView/ItemsviewItemInvocationSelection.h"

    return gallery::controlExample({
        .header = gallery::snippet(invocationHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(invocationCode),
    });
}

}  // namespace

static wxl::FrameworkElement itemsViewPage() {
    return StackPanel {basicView(), swappable(), invocation()};
}

REGISTER_GALLERY_PAGE(L"ItemsView", itemsViewPage);
