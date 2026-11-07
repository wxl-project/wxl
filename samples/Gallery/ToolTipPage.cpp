// Страница ToolTip -- ToolTipPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/ToolTip/ToolTipButton.html"};
gallery::LazyLoadedText simpleCode{"Snippets/ToolTip/ToolTipButton.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/ToolTip/ToolTipButton.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText placedHeader{"Snippets/ToolTip/ToolTipPlacement.html"};
gallery::LazyLoadedText placedCode{"Snippets/ToolTip/ToolTipPlacement.h"};

FrameworkElement placed() {
    return gallery::controlExample({
        .header = gallery::snippet(placedHeader),
        .example =
#include "Snippets/ToolTip/ToolTipPlacement.h"
        ,
        .code = gallery::snippet(placedCode),
    });
}

gallery::LazyLoadedText imageHeader{"Snippets/ToolTip/ToolTipImage.html"};
gallery::LazyLoadedText imageCode{"Snippets/ToolTip/ToolTipImage.h"};

FrameworkElement image() {
    return gallery::controlExample({
        .header = gallery::snippet(imageHeader),
        .example =
#include "Snippets/ToolTip/ToolTipImage.h"
        ,
        .code = gallery::snippet(imageCode),
    });
}

}  // namespace

static wxl::FrameworkElement toolTipPage() {
    return StackPanel {simple(), placed(), image()};
}

REGISTER_GALLERY_PAGE(L"ToolTip", toolTipPage);
