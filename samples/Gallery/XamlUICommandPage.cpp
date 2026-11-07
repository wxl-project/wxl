// Страница XamlUICommand -- XamlUICommandPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText reusableHeader{"Snippets/XamlUICommand/Reusable.html"};
gallery::LazyLoadedText reusableCode{"Snippets/XamlUICommand/Reusable.h"};

FrameworkElement reusable() {
#include "Snippets/XamlUICommand/Reusable.h"

    return gallery::controlExample({
        .header = gallery::snippet(reusableHeader),
        .example = example,
        .code = gallery::snippet(reusableCode),
    });
}

}  // namespace

static wxl::FrameworkElement xamlUICommandPage() {
    return StackPanel {reusable()};
}

REGISTER_GALLERY_PAGE(L"XamlUICommand", xamlUICommandPage);
