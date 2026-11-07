// Страница Viewbox — ViewboxPage оригинала: один пример.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText contentHeader{"Snippets/Viewbox/ContentInsideViewbox.html"};
gallery::LazyLoadedText contentCode{"Snippets/Viewbox/ContentInsideViewbox.h"};

FrameworkElement insideViewbox() {
#include "Snippets/Viewbox/ContentInsideViewbox.h"

    return gallery::controlExample({
        .header = gallery::snippet(contentHeader),
        .example = viewbox,
        .options = {StackPanel {width = 200, size, stretchGroup, directionGroup}},
        .code = gallery::snippet(contentCode),
    });
}

}  // namespace

static wxl::FrameworkElement viewboxPage() {
    return StackPanel {insideViewbox()};
}

REGISTER_GALLERY_PAGE(L"Viewbox", viewboxPage);
