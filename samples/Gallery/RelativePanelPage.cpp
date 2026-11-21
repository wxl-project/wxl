// Страница RelativePanel — RelativePanelPage оригинала: один пример.

#include "Pages.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText controlHeader{"Snippets/RelativePanel/RelativePanelControl.html"};
gallery::LazyLoadedText controlCode{"Snippets/RelativePanel/RelativePanelControl.h"};

FrameworkElement control() {
    using namespace wxl::dsl;
#include "Snippets/RelativePanel/RelativePanelControl.h"

    return gallery::controlExample({
        .header = gallery::snippet(controlHeader),
        .example = panel,
        .code = gallery::snippet(controlCode),
    });
}

}  // namespace

static wxl::FrameworkElement relativePanelPage() {
    return StackPanel {control()};
}

REGISTER_GALLERY_PAGE(L"RelativePanel", relativePanelPage);
