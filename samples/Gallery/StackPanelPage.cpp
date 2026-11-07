// Страница StackPanel — StackPanelPage оригинала: один пример.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText controlHeader{"Snippets/StackPanel/StackPanelControl.html"};
gallery::LazyLoadedText controlCode{"Snippets/StackPanel/StackPanelControl.h"};

FrameworkElement control() {
#include "Snippets/StackPanel/StackPanelControl.h"

    return gallery::controlExample({
        .header = gallery::snippet(controlHeader),
        .example = panel,
        .options = {StackPanel {spacing = 12.0, orientationGroup, gapSlider}},
        .code = gallery::snippet(controlCode),
    });
}

}  // namespace

static wxl::FrameworkElement stackPanelPage() {
    return StackPanel {control()};
}

REGISTER_GALLERY_PAGE(L"StackPanel", stackPanelPage);
