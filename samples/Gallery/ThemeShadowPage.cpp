// Страница ThemeShadow -- ThemeShadowPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText appliedBorderHeader{"Snippets/ThemeShadow/AppliedBorder.html"};
gallery::LazyLoadedText appliedBorderCode{"Snippets/ThemeShadow/AppliedBorder.h"};

FrameworkElement appliedBorder() {
#include "Snippets/ThemeShadow/AppliedBorder.h"

    return gallery::controlExample({
        .header = gallery::snippet(appliedBorderHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(appliedBorderCode),
    });
}

}  // namespace

static wxl::FrameworkElement themeShadowPage() {
    return StackPanel {appliedBorder()};
}

REGISTER_GALLERY_PAGE(L"ThemeShadow", themeShadowPage);
