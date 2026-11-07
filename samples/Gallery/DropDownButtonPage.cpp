// Страница DropDownButton — DropDownButtonPage оригинала: два примера.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/DropDownButton/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/DropDownButton/Simple.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/DropDownButton/Simple.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText iconHeader{"Snippets/DropDownButton/Icon.html"};
gallery::LazyLoadedText iconCode{"Snippets/DropDownButton/Icon.h"};

FrameworkElement withIcon() {
    return gallery::controlExample({
        .header = gallery::snippet(iconHeader),
        .example =
#include "Snippets/DropDownButton/Icon.h"
        ,
        .code = gallery::snippet(iconCode),
    });
}

}  // namespace

static wxl::FrameworkElement dropDownButtonPage() {
    return StackPanel {simple(), withIcon()};
}

REGISTER_GALLERY_PAGE(L"DropDownButton", dropDownButtonPage);
