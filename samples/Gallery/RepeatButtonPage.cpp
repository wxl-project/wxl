// Страница RepeatButton — RepeatButtonPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/RepeatButton/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/RepeatButton/Simple.h"};

FrameworkElement simple() {
#include "Snippets/RepeatButton/Simple.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = StackPanel {orientation.horizontal, button, output},
        .options = {disable},
        .code = gallery::snippet(simpleCode),
    });
}

}  // namespace

static wxl::FrameworkElement repeatButtonPage() {
    return StackPanel {simple()};
}

REGISTER_GALLERY_PAGE(L"RepeatButton", repeatButtonPage);
