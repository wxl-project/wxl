// Страница ToggleButton — ToggleButtonPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/ToggleButton/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/ToggleButton/Simple.h"};

FrameworkElement simple() {
#include "Snippets/ToggleButton/Simple.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = StackPanel {vAlign.top, orientation.horizontal, toggle},
        .output = {output},
        .options = {disable},
        .code = gallery::snippet(simpleCode),
    });
}

}  // namespace

static wxl::FrameworkElement toggleButtonPage() {
    return StackPanel {simple()};
}

REGISTER_GALLERY_PAGE(L"ToggleButton", toggleButtonPage);
