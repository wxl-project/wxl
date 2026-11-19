// Страница ContentDialog -- ContentDialogPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/ContentDialog/BasicContentDialogContent.html"};
gallery::LazyLoadedText basicCode{"Snippets/ContentDialog/BasicContentDialogContent.h"};

FrameworkElement basic() {
#include "Snippets/ContentDialog/BasicContentDialogContent.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText withoutDefaultHeader{"Snippets/ContentDialog/ContentDialogWithoutDefault.html"};
gallery::LazyLoadedText withoutDefaultCode{"Snippets/ContentDialog/ContentDialogWithoutDefault.h"};

FrameworkElement withoutDefault() {
#include "Snippets/ContentDialog/ContentDialogWithoutDefault.h"

    return gallery::controlExample({
        .header = gallery::snippet(withoutDefaultHeader),
        .example = example,
        .code = gallery::snippet(withoutDefaultCode),
    });
}

}  // namespace

static wxl::FrameworkElement contentDialogPage() {
    return StackPanel {basic(), withoutDefault()};
}

REGISTER_GALLERY_PAGE(L"ContentDialog", contentDialogPage);
