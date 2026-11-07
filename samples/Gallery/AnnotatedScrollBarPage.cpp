// Страница AnnotatedScrollBar -- AnnotatedScrollBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText linkedHeader{"Snippets/AnnotatedScrollBar/LinkedScrollView.html"};
gallery::LazyLoadedText linkedCode{"Snippets/AnnotatedScrollBar/LinkedScrollView.h"};

FrameworkElement linked() {
#include "Snippets/AnnotatedScrollBar/LinkedScrollView.h"

    return gallery::controlExample({
        .header = gallery::snippet(linkedHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(linkedCode),
    });
}

}  // namespace

static wxl::FrameworkElement annotatedScrollBarPage() {
    return StackPanel {linked()};
}

REGISTER_GALLERY_PAGE(L"AnnotatedScrollBar", annotatedScrollBarPage);
