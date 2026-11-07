// Страница Border — BorderPage оригинала: один пример.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText aroundHeader{"Snippets/Border/BorderAroundTextblock.html"};
gallery::LazyLoadedText aroundCode{"Snippets/Border/BorderAroundTextblock.h"};

FrameworkElement around() {
#include "Snippets/Border/BorderAroundTextblock.h"

    return gallery::controlExample({
        .header = gallery::snippet(aroundHeader),
        .example = border,
        .options = {thickness, Grid {ColumnDefinitions<u"*,*">, columnSpacing = 8.0, backgroundChoice, brushChoice}},
        .code = gallery::snippet(aroundCode),
    });
}

}  // namespace

static wxl::FrameworkElement borderPage() {
    return StackPanel {around()};
}

REGISTER_GALLERY_PAGE(L"Border", borderPage);
