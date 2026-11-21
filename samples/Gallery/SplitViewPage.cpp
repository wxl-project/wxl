// Страница SplitView — SplitViewPage оригинала: один пример.
//
// Шаблоны пунктов оригинала (значок слева и справа, `VisualState`) здесь —
// пересборка списка при смене стороны панели.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/SplitView/BasicSplitView.html"};
gallery::LazyLoadedText basicCode{"Snippets/SplitView/BasicSplitView.h"};

FrameworkElement basic() {
    using namespace wxl::dsl;
#include "Snippets/SplitView/BasicSplitView.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .options = {StackPanel {togglePane, placementSwitch, displayModeBox, paneBackgroundBox, openPaneLengthSlider, compactPaneLengthSlider}},
        .code = gallery::snippet(basicCode),
    });
}

}  // namespace

static wxl::FrameworkElement splitViewPage() {
    return StackPanel {basic()};
}

REGISTER_GALLERY_PAGE(L"SplitView", splitViewPage);
