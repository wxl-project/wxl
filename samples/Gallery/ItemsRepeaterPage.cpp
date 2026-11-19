// Страница ItemsRepeater -- ItemsRepeaterPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <algorithm>
#include <memory>
#include <vector>
#include "Announce.h"
#include <wxl/Microsoft.UI.Xaml.Automation.Peers.Enums.h>
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include "KeyedIndexList.h"
#include "Layouts.h"
#include "Resource.h"
#include "RepeaterData.h"
#include <wxl/Microsoft.UI.Dispatching.h>
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Hosting.h>
#include <wxl/Microsoft.UI.Xaml.Input.h>
#include <wxl/Microsoft.UI.Xaml.h>
#include <wxl/Microsoft.UI.Composition.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText barsHeader{"Snippets/ItemsRepeater/ItemsRepeaterBasicNonInteractiveItems.html"};
gallery::LazyLoadedText barsCode{"Snippets/ItemsRepeater/ItemsRepeaterBasicNonInteractiveItems.h"};

FrameworkElement bars() {
#include "Snippets/ItemsRepeater/ItemsRepeaterBasicNonInteractiveItems.h"

    return gallery::controlExample({
        .header = gallery::snippet(barsHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(barsCode),
    });
}

gallery::LazyLoadedText virtualizingHeader{"Snippets/ItemsRepeater/ItemsRepeaterVirtualizingScrollableListItems.html"};
gallery::LazyLoadedText virtualizingCode{"Snippets/ItemsRepeater/ItemsRepeaterVirtualizingScrollableListItems.h"};

FrameworkElement virtualizing() {
#include "Snippets/ItemsRepeater/ItemsRepeaterVirtualizingScrollableListItems.h"

    return gallery::controlExample({
        .header = gallery::snippet(virtualizingHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(virtualizingCode),
    });
}

gallery::LazyLoadedText flowHeader{"Snippets/ItemsRepeater/FlowLayoutVirtualization.html"};
gallery::LazyLoadedText flowCode{"Snippets/ItemsRepeater/FlowLayoutVirtualization.h"};

FrameworkElement flow() {
#include "Snippets/ItemsRepeater/FlowLayoutVirtualization.h"

    return gallery::controlExample({
        .header = gallery::snippet(flowHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(flowCode),
    });
}

gallery::LazyLoadedText mixedHeader{"Snippets/ItemsRepeater/ItemsrepeaterMixedTypeCollection.html"};
gallery::LazyLoadedText mixedCode{"Snippets/ItemsRepeater/ItemsrepeaterMixedTypeCollection.h"};

FrameworkElement mixed() {
#include "Snippets/ItemsRepeater/ItemsrepeaterMixedTypeCollection.h"

    return gallery::controlExample({
        .header = gallery::snippet(mixedHeader),
        .example = example,
        .code = gallery::snippet(mixedCode),
    });
}

gallery::LazyLoadedText nestedHeader{"Snippets/ItemsRepeater/LayingOutNestedItemsrepeaters.html"};
gallery::LazyLoadedText nestedCode{"Snippets/ItemsRepeater/LayingOutNestedItemsrepeaters.h"};

FrameworkElement nested() {
#include "Snippets/ItemsRepeater/LayingOutNestedItemsrepeaters.h"

    return gallery::controlExample({
        .header = gallery::snippet(nestedHeader),
        .example = example,
        .code = gallery::snippet(nestedCode),
    });
}

gallery::LazyLoadedText animatedHeader{"Snippets/ItemsRepeater/ItemsRepeaterAnimatedScrollingContentDisplay.html"};
gallery::LazyLoadedText animatedCode{"Snippets/ItemsRepeater/ItemsRepeaterAnimatedScrollingContentDisplay.h"};

FrameworkElement animated() {
#include "Snippets/ItemsRepeater/ItemsRepeaterAnimatedScrollingContentDisplay.h"

    return gallery::controlExample({
        .header = gallery::snippet(animatedHeader),
        .example = example,
        .code = gallery::snippet(animatedCode),
    });
}

gallery::LazyLoadedText contentHeavyHeader{"Snippets/ItemsRepeater/ItemsRepeaterVirtualizedContentHeavyLayout.html"};
gallery::LazyLoadedText contentHeavyCode{"Snippets/ItemsRepeater/ItemsRepeaterVirtualizedContentHeavyLayout.h"};

FrameworkElement contentHeavy() {
#include "Snippets/ItemsRepeater/ItemsRepeaterVirtualizedContentHeavyLayout.h"

    return gallery::controlExample({
        .header = gallery::snippet(contentHeavyHeader),
        .example = example,
        .code = gallery::snippet(contentHeavyCode),
    });
}

}  // namespace

static wxl::FrameworkElement itemsRepeaterPage() {
    return StackPanel {bars(), virtualizing(), flow(), mixed(), nested(), animated(), contentHeavy()};
}

REGISTER_GALLERY_PAGE(L"ItemsRepeater", itemsRepeaterPage);
