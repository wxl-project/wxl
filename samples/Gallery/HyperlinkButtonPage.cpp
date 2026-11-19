// Страница HyperlinkButton — HyperlinkButtonPage оригинала: два примера.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText navigateHeader{"Snippets/HyperlinkButton/Navigate.html"};
gallery::LazyLoadedText navigateCode{"Snippets/HyperlinkButton/Navigate.h"};

FrameworkElement navigateExample() {
#include "Snippets/HyperlinkButton/Navigate.h"

    return gallery::controlExample({
        .header = gallery::snippet(navigateHeader),
        .example = link,
        .options = {disable},
        .code = gallery::snippet(navigateCode),
    });
}

gallery::LazyLoadedText clickHeader{"Snippets/HyperlinkButton/Click.html"};
gallery::LazyLoadedText clickCode{"Snippets/HyperlinkButton/Click.h"};

FrameworkElement clickExample() {
    return gallery::controlExample({
        .header = gallery::snippet(clickHeader),
        .example =
#include "Snippets/HyperlinkButton/Click.h"
        ,
        .code = gallery::snippet(clickCode),
    });
}

}  // namespace

static wxl::FrameworkElement hyperlinkButtonPage() {
    return StackPanel {navigateExample(), clickExample()};
}

REGISTER_GALLERY_PAGE(L"HyperlinkButton", hyperlinkButtonPage);
