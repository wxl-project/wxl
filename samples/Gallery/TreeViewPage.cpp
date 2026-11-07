// Страница TreeView -- TreeViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText dragDropHeader{"Snippets/TreeView/SimpleTreeviewDragDrop.html"};
gallery::LazyLoadedText dragDropCode{"Snippets/TreeView/SimpleTreeviewDragDrop.h"};

FrameworkElement dragDrop() {
#include "Snippets/TreeView/SimpleTreeviewDragDrop.h"

    return gallery::controlExample({
        .header = gallery::snippet(dragDropHeader),
        .example = example,
        .code = gallery::snippet(dragDropCode),
    });
}

gallery::LazyLoadedText multipleHeader{"Snippets/TreeView/TreeviewMultiSelectionEnabled.html"};
gallery::LazyLoadedText multipleCode{"Snippets/TreeView/TreeviewMultiSelectionEnabled.h"};

FrameworkElement multiple() {
#include "Snippets/TreeView/TreeviewMultiSelectionEnabled.h"

    return gallery::controlExample({
        .header = gallery::snippet(multipleHeader),
        .example = example,
        .code = gallery::snippet(multipleCode),
    });
}

gallery::LazyLoadedText boundHeader{"Snippets/TreeView/TreeviewDatabindingItemsource.html"};
gallery::LazyLoadedText boundCode{"Snippets/TreeView/TreeviewDatabindingItemsource.h"};

FrameworkElement bound() {
#include "Snippets/TreeView/TreeviewDatabindingItemsource.h"

    return gallery::controlExample({
        .header = gallery::snippet(boundHeader),
        .example = example,
        .code = gallery::snippet(boundCode),
    });
}

gallery::LazyLoadedText selectorHeader{"Snippets/TreeView/TreeviewItemtemplateselector.html"};
gallery::LazyLoadedText selectorCode{"Snippets/TreeView/TreeviewItemtemplateselector.h"};

FrameworkElement selector() {
#include "Snippets/TreeView/TreeviewItemtemplateselector.h"

    return gallery::controlExample({
        .header = gallery::snippet(selectorHeader),
        .example = example,
        .code = gallery::snippet(selectorCode),
    });
}

}  // namespace

static wxl::FrameworkElement treeViewPage() {
    return StackPanel {dragDrop(), multiple(), bound(), selector()};
}

REGISTER_GALLERY_PAGE(L"TreeView", treeViewPage);
