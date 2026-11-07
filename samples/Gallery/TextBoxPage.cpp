// Страница TextBox — TextBoxPage оригинала: четыре примера.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/TextBox/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/TextBox/Simple.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/TextBox/Simple.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText headerHeader{"Snippets/TextBox/HeaderPlaceholderText.html"};
gallery::LazyLoadedText headerCode{"Snippets/TextBox/HeaderPlaceholderText.h"};

FrameworkElement headerPlaceholder() {
    return gallery::controlExample({
        .header = gallery::snippet(headerHeader),
        .example =
#include "Snippets/TextBox/HeaderPlaceholderText.h"
        ,
        .code = gallery::snippet(headerCode),
    });
}

gallery::LazyLoadedText readOnlyHeader{"Snippets/TextBox/ReadOnlyVarious.html"};
gallery::LazyLoadedText readOnlyCode{"Snippets/TextBox/ReadOnlyVarious.h"};

FrameworkElement readOnlyVarious() {
    return gallery::controlExample({
        .header = gallery::snippet(readOnlyHeader),
        .example =
#include "Snippets/TextBox/ReadOnlyVarious.h"
        ,
        .code = gallery::snippet(readOnlyCode),
    });
}

gallery::LazyLoadedText multiHeader{"Snippets/TextBox/MultiLineSpell.html"};
gallery::LazyLoadedText multiCode{"Snippets/TextBox/MultiLineSpell.h"};

FrameworkElement multiLine() {
    return gallery::controlExample({
        .header = gallery::snippet(multiHeader),
        .example =
#include "Snippets/TextBox/MultiLineSpell.h"
        ,
        .code = gallery::snippet(multiCode),
    });
}

}  // namespace

static wxl::FrameworkElement textBoxPage() {
    return StackPanel {simple(), headerPlaceholder(), readOnlyVarious(), multiLine()};
}

REGISTER_GALLERY_PAGE(L"TextBox", textBoxPage);
