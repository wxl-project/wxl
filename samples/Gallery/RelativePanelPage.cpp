// Страница RelativePanel — RelativePanelPage оригинала: один пример.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t controlHeader[] = {
#include "Snippets/RelativePanel/RelativePanelControl.html.embed"
};
constexpr char8_t controlCode[] = {
#include "Snippets/RelativePanel/RelativePanelControl.h.embed"
};

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
