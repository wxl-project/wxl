// Страница TextBlock — TextBlockPage оригинала: пять примеров.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/TextBlock/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/TextBlock/Simple.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/TextBlock/Simple.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText styleHeader{"Snippets/TextBlock/StyleApplied.html"};
gallery::LazyLoadedText styleCode{"Snippets/TextBlock/StyleApplied.h"};

FrameworkElement styleApplied() {
#include "Snippets/TextBlock/StyleApplied.h"

    return gallery::controlExample({
        .header = gallery::snippet(styleHeader),
        .example = styled,
        .code = gallery::snippet(styleCode),
    });
}

gallery::LazyLoadedText variousHeader{"Snippets/TextBlock/VariousProperties.html"};
gallery::LazyLoadedText variousCode{"Snippets/TextBlock/VariousProperties.h"};

FrameworkElement various() {
    return gallery::controlExample({
        .header = gallery::snippet(variousHeader),
        .example =
#include "Snippets/TextBlock/VariousProperties.h"
        ,
        .code = gallery::snippet(variousCode),
    });
}

gallery::LazyLoadedText inlineHeader{"Snippets/TextBlock/InlineTextElements.html"};
gallery::LazyLoadedText inlineCode{"Snippets/TextBlock/InlineTextElements.h"};

FrameworkElement inlineElements() {
    return gallery::controlExample({
        .header = gallery::snippet(inlineHeader),
        .example =
#include "Snippets/TextBlock/InlineTextElements.h"
        ,
        .code = gallery::snippet(inlineCode),
    });
}

gallery::LazyLoadedText selectableHeader{"Snippets/TextBlock/Selectable.html"};
gallery::LazyLoadedText selectableCode{"Snippets/TextBlock/Selectable.h"};

FrameworkElement selectable() {
#include "Snippets/TextBlock/Selectable.h"

    return gallery::controlExample({
        .header = gallery::snippet(selectableHeader),
        .example = text,
        .options = {option},
        .code = gallery::snippet(selectableCode),
    });
}

}  // namespace

static wxl::FrameworkElement textBlockPage() {
    return StackPanel {simple(), styleApplied(), various(), inlineElements(), selectable()};
}

REGISTER_GALLERY_PAGE(L"TextBlock", textBlockPage);
