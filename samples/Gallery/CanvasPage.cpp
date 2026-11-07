// Страница Canvas — CanvasPage оригинала: один пример.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText controlHeader{"Snippets/Canvas/CanvasControl.html"};
gallery::LazyLoadedText controlCode{"Snippets/Canvas/CanvasControl.h"};

FrameworkElement control() {
#include "Snippets/Canvas/CanvasControl.h"

    return gallery::controlExample({
        .header = gallery::snippet(controlHeader),
        .example = canvas,
        .options = {StackPanel {orientation.horizontal, topSlider, StackPanel {Margin {16, 0, 0, 0}, leftSlider, zSlider}}},
        .code = gallery::snippet(controlCode),
    });
}

}  // namespace

static wxl::FrameworkElement canvasPage() {
    return StackPanel {control()};
}

REGISTER_GALLERY_PAGE(L"Canvas", canvasPage);
