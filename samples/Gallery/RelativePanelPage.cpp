// Страница RelativePanel — RelativePanelPage оригинала: один пример.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText controlHeader{"Snippets/RelativePanel/RelativePanelControl.html"};
gallery::LazyLoadedText controlCode{"Snippets/RelativePanel/RelativePanelControl.h"};

FrameworkElement control() {
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
