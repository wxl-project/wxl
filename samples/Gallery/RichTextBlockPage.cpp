// Страница RichTextBlock — RichTextBlockPage оригинала.
//
// Четвёртый пример оригинала (RichtextblockCustomTexthighlighting:
// TextHighlighter и диапазоны TextRange) не перенесён: TextRange из
// Microsoft.UI.Xaml.Documents — структура с тем же именем, что у обёртки
// Microsoft.UI.Text.TextRange, и коллекция структур ещё не проецируется.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/RichTextBlock/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/RichTextBlock/Simple.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/RichTextBlock/Simple.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText selectionHeader{"Snippets/RichTextBlock/CustomSelectionHighlight.html"};
gallery::LazyLoadedText selectionCode{"Snippets/RichTextBlock/CustomSelectionHighlight.h"};

FrameworkElement customSelection() {
    return gallery::controlExample({
        .header = gallery::snippet(selectionHeader),
        .example =
#include "Snippets/RichTextBlock/CustomSelectionHighlight.h"
        ,
        .code = gallery::snippet(selectionCode),
    });
}

gallery::LazyLoadedText overflowHeader{"Snippets/RichTextBlock/Overflow.html"};
gallery::LazyLoadedText overflowCode{"Snippets/RichTextBlock/Overflow.h"};

FrameworkElement overflow() {
#include "Snippets/RichTextBlock/Overflow.h"

    return gallery::controlExample({
        .header = gallery::snippet(overflowHeader),
        .example = columns,
        .code = gallery::snippet(overflowCode),
    });
}

}  // namespace

static wxl::FrameworkElement richTextBlockPage() {
    return StackPanel {simple(), customSelection(), overflow()};
}

REGISTER_GALLERY_PAGE(L"RichTextBlock", richTextBlockPage);
