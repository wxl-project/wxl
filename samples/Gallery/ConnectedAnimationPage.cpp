// Страница ConnectedAnimation -- ConnectedAnimationPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <memory>
#include <vector>
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include <wxl/Microsoft.UI.Dispatching.h>
#include "MotionPages.h"
#include "PagedFrame.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.Animation.h>
#include <wxl/Microsoft.UI.Xaml.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText listPageHeader{"Snippets/ConnectedAnimation/ConnectedAnimationListPage.html"};
gallery::LazyLoadedText listPageCode{"Snippets/ConnectedAnimation/ConnectedAnimationListPage.h"};

FrameworkElement listPage() {
#include "Snippets/ConnectedAnimation/ConnectedAnimationListPage.h"

    return gallery::controlExample({
        .header = gallery::snippet(listPageHeader),
        .example = example,
        .code = gallery::snippet(listPageCode),
    });
}

gallery::LazyLoadedText sameElementsHeader{"Snippets/ConnectedAnimation/ConnectedAnimationElementsSame.html"};
gallery::LazyLoadedText sameElementsCode{"Snippets/ConnectedAnimation/ConnectedAnimationElementsSame.h"};

FrameworkElement sameElements() {
#include "Snippets/ConnectedAnimation/ConnectedAnimationElementsSame.h"

    return gallery::controlExample({
        .header = gallery::snippet(sameElementsHeader),
        .example = example,
        .code = gallery::snippet(sameElementsCode),
    });
}

gallery::LazyLoadedText simpleHeader{"Snippets/ConnectedAnimation/SimpleConnectedAnimation.html"};
gallery::LazyLoadedText simpleCode{"Snippets/ConnectedAnimation/SimpleConnectedAnimation.h"};

FrameworkElement simple() {
#include "Snippets/ConnectedAnimation/SimpleConnectedAnimation.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText itemsRepeaterHeader{"Snippets/ConnectedAnimation/ConnectedAnimationItemsrepeater.html"};
gallery::LazyLoadedText itemsRepeaterCode{"Snippets/ConnectedAnimation/ConnectedAnimationItemsrepeater.h"};

FrameworkElement itemsRepeater() {
#include "Snippets/ConnectedAnimation/ConnectedAnimationItemsrepeater.h"

    return gallery::controlExample({
        .header = gallery::snippet(itemsRepeaterHeader),
        .example = example,
        .code = gallery::snippet(itemsRepeaterCode),
    });
}

}  // namespace

static wxl::FrameworkElement connectedAnimationPage() {
    return StackPanel {listPage(), sameElements(), simple(), itemsRepeater()};
}

REGISTER_GALLERY_PAGE(L"ConnectedAnimation", connectedAnimationPage);
