// Страница ListView -- ListViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <algorithm>
#include <cwctype>
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include "Contact.h"
#include <wxl/Windows.ApplicationModel.DataTransfer.h>
#include "CustomDataObject.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>
#include <wxl/Microsoft.UI.Xaml.Shapes.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicListHeader{"Snippets/ListView/BasicListviewSimpleDatatemplate.html"};
gallery::LazyLoadedText basicListCode{"Snippets/ListView/BasicListviewSimpleDatatemplate.h"};

FrameworkElement basicList() {
#include "Snippets/ListView/BasicListviewSimpleDatatemplate.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicListHeader),
        .example = example,
        .code = gallery::snippet(basicListCode),
    });
}

gallery::LazyLoadedText selectionHeader{"Snippets/ListView/ListviewSelectionSupport.html"};
gallery::LazyLoadedText selectionCode{"Snippets/ListView/ListviewSelectionSupport.h"};

FrameworkElement selection() {
#include "Snippets/ListView/ListviewSelectionSupport.h"

    return gallery::controlExample({
        .header = gallery::snippet(selectionHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(selectionCode),
    });
}

gallery::LazyLoadedText imagesHeader{"Snippets/ListView/ListviewImages.html"};
gallery::LazyLoadedText imagesCode{"Snippets/ListView/ListviewImages.h"};

FrameworkElement images() {
#include "Snippets/ListView/ListviewImages.h"

    return gallery::controlExample({
        .header = gallery::snippet(imagesHeader),
        .example = example,
        .code = gallery::snippet(imagesCode),
    });
}

gallery::LazyLoadedText filteringHeader{"Snippets/ListView/ListviewFiltering.html"};
gallery::LazyLoadedText filteringCode{"Snippets/ListView/ListviewFiltering.h"};

FrameworkElement filtering() {
#include "Snippets/ListView/ListviewFiltering.h"

    return gallery::controlExample({
        .header = gallery::snippet(filteringHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(filteringCode),
    });
}

gallery::LazyLoadedText menusHeader{"Snippets/ListView/ListviewContextMenus.html"};
gallery::LazyLoadedText menusCode{"Snippets/ListView/ListviewContextMenus.h"};

FrameworkElement menus() {
#include "Snippets/ListView/ListviewContextMenus.h"

    return gallery::controlExample({
        .header = gallery::snippet(menusHeader),
        .example = example,
        .code = gallery::snippet(menusCode),
    });
}

gallery::LazyLoadedText messagingHeader{"Snippets/ListView/ListviewMessagingDataLogging.html"};
gallery::LazyLoadedText messagingCode{"Snippets/ListView/ListviewMessagingDataLogging.h"};

FrameworkElement messaging() {
#include "Snippets/ListView/ListviewMessagingDataLogging.h"

    return gallery::controlExample({
        .header = gallery::snippet(messagingHeader),
        .example = example,
        .code = gallery::snippet(messagingCode),
    });
}

gallery::LazyLoadedText restoringHeader{"Snippets/ListView/ListviewRestoreScrollPosition.html"};
gallery::LazyLoadedText restoringCode{"Snippets/ListView/ListviewRestoreScrollPosition.h"};

FrameworkElement restoring() {
#include "Snippets/ListView/ListviewRestoreScrollPosition.h"

    return gallery::controlExample({
        .header = gallery::snippet(restoringHeader),
        .example = example,
        .code = gallery::snippet(restoringCode),
    });
}

gallery::LazyLoadedText scrollingHeader{"Snippets/ListView/ListviewScrollIntoView.html"};
gallery::LazyLoadedText scrollingCode{"Snippets/ListView/ListviewScrollIntoView.h"};

FrameworkElement scrolling() {
#include "Snippets/ListView/ListviewScrollIntoView.h"

    return gallery::controlExample({
        .header = gallery::snippet(scrollingHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(scrollingCode),
    });
}

gallery::LazyLoadedText draggingHeader{"Snippets/ListView/ListviewsDragDropReordering.html"};
gallery::LazyLoadedText draggingCode{"Snippets/ListView/ListviewsDragDropReordering.h"};

FrameworkElement dragging() {
#include "Snippets/ListView/ListviewsDragDropReordering.h"

    return gallery::controlExample({
        .header = gallery::snippet(draggingHeader),
        .example = example,
        .code = gallery::snippet(draggingCode),
    });
}

gallery::LazyLoadedText groupedHeader{"Snippets/ListView/ListviewGroupedHeaders.html"};
gallery::LazyLoadedText groupedCode{"Snippets/ListView/ListviewGroupedHeaders.h"};

FrameworkElement grouped() {
#include "Snippets/ListView/ListviewGroupedHeaders.h"

    return gallery::controlExample({
        .header = gallery::snippet(groupedHeader),
        .example = example,
        .code = gallery::snippet(groupedCode),
    });
}

}  // namespace

static wxl::FrameworkElement listViewPage() {
    return StackPanel {basicList(), selection(), images(), filtering(), menus(), messaging(), restoring(), scrolling(), dragging(), grouped()};
}

REGISTER_GALLERY_PAGE(L"ListView", listViewPage);
