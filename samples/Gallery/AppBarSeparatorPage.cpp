// Страница AppBarSeparator -- AppBarSeparatorPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText separatedHeader{"Snippets/AppBarSeparator/Separated.html"};
gallery::LazyLoadedText separatedCode{"Snippets/AppBarSeparator/Separated.h"};

FrameworkElement separated() {
    return gallery::controlExample({
        .header = gallery::snippet(separatedHeader),
        .example =
#include "Snippets/AppBarSeparator/Separated.h"
        ,
        .code = gallery::snippet(separatedCode),
    });
}

}  // namespace

static wxl::FrameworkElement appBarSeparatorPage() {
    return StackPanel {separated()};
}

REGISTER_GALLERY_PAGE(L"AppBarSeparator", appBarSeparatorPage);
