// Страница WrapPanel -- WrapPanelPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/WrapPanel/Basic.html"};
gallery::LazyLoadedText basicCode{"Snippets/WrapPanel/Basic.h"};

FrameworkElement basic() {
    using namespace wxl::dsl;
#include "Snippets/WrapPanel/Basic.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = layoutHost,
        .options = {options},
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText stretchHeader{"Snippets/WrapPanel/Stretch.html"};
gallery::LazyLoadedText stretchCode{"Snippets/WrapPanel/Stretch.h"};

FrameworkElement lastItem() {
    using namespace wxl::dsl;
#include "Snippets/WrapPanel/Stretch.h"

    return gallery::controlExample({
        .header = gallery::snippet(stretchHeader),
        .example = panelHost,
        .options = {stretchToggle},
        .code = gallery::snippet(stretchCode),
    });
}

}  // namespace

static wxl::FrameworkElement wrapPanelPage() {
    return StackPanel {basic(), lastItem()};
}

REGISTER_GALLERY_PAGE(L"WrapPanel", wrapPanelPage);
