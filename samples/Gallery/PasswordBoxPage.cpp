// Страница PasswordBox — PasswordBoxPage оригинала: три примера.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/PasswordBox/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/PasswordBox/Simple.h"};

FrameworkElement simple() {
#include "Snippets/PasswordBox/Simple.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = StackPanel {box, output},
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText headerHeader{"Snippets/PasswordBox/HeaderPlaceholderText.html"};
gallery::LazyLoadedText headerCode{"Snippets/PasswordBox/HeaderPlaceholderText.h"};

FrameworkElement headerPlaceholder() {
    return gallery::controlExample({
        .header = gallery::snippet(headerHeader),
        .example =
#include "Snippets/PasswordBox/HeaderPlaceholderText.h"
        ,
        .code = gallery::snippet(headerCode),
    });
}

gallery::LazyLoadedText revealHeader{"Snippets/PasswordBox/RevealMode.html"};
gallery::LazyLoadedText revealCode{"Snippets/PasswordBox/RevealMode.h"};

FrameworkElement revealMode() {
#include "Snippets/PasswordBox/RevealMode.h"

    return gallery::controlExample({
        .header = gallery::snippet(revealHeader),
        .example = StackPanel {orientation.horizontal, box, reveal},
        .code = gallery::snippet(revealCode),
    });
}

}  // namespace

static wxl::FrameworkElement passwordBoxPage() {
    return StackPanel {simple(), headerPlaceholder(), revealMode()};
}

REGISTER_GALLERY_PAGE(L"PasswordBox", passwordBoxPage);
