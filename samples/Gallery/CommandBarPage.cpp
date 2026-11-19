// Страница CommandBar -- CommandBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText labelsSideHeader{"Snippets/CommandBar/LabelsSide.html"};
gallery::LazyLoadedText labelsSideCode{"Snippets/CommandBar/LabelsSide.h"};

FrameworkElement labelsSide() {
#include "Snippets/CommandBar/LabelsSide.h"

    return gallery::controlExample({
        .header = gallery::snippet(labelsSideHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(labelsSideCode),
    });
}

}  // namespace

static wxl::FrameworkElement commandBarPage() {
    return StackPanel {labelsSide()};
}

REGISTER_GALLERY_PAGE(L"CommandBar", commandBarPage);
