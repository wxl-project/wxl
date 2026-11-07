// Страница Grid — GridPage оригинала: один пример.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText threeHeader{"Snippets/Grid/ThreeByThreeGrid.html"};
gallery::LazyLoadedText threeCode{"Snippets/Grid/ThreeByThreeGrid.h"};

FrameworkElement three() {
#include "Snippets/Grid/ThreeByThreeGrid.h"

    return gallery::controlExample({
        .header = gallery::snippet(threeHeader),
        .example = grid,
        .options = {options},
        .code = gallery::snippet(threeCode),
    });
}

}  // namespace

static wxl::FrameworkElement gridPage() {
    return StackPanel {three()};
}

REGISTER_GALLERY_PAGE(L"Grid", gridPage);
