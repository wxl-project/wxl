// Страница SplitView — SplitViewPage оригинала: один пример.
//
// Шаблоны пунктов оригинала (значок слева и справа, `VisualState`) здесь —
// пересборка списка при смене стороны панели.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t basicHeader[] = {
#include "Snippets/SplitView/BasicSplitView.html.embed"
};
constexpr char8_t basicCode[] = {
#include "Snippets/SplitView/BasicSplitView.h.embed"
};

FrameworkElement basic() {
#include "Snippets/SplitView/BasicSplitView.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .options = {StackPanel {togglePane, placement, displayMode, paneBackground, openPaneLength, compactPaneLength}},
        .code = gallery::snippet(basicCode),
    });
}

}  // namespace

static wxl::FrameworkElement splitViewPage() {
    return StackPanel {basic()};
}

REGISTER_GALLERY_PAGE(L"SplitView", splitViewPage);
