// Страница Popup -- PopupPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText offsetExampleHeader{"Snippets/Popup/PopupOffsetPositioning.html"};
gallery::LazyLoadedText offsetExampleCode{"Snippets/Popup/PopupOffsetPositioning.h"};

FrameworkElement offsetExample() {
#include "Snippets/Popup/PopupOffsetPositioning.h"

    return gallery::controlExample({
        .header = gallery::snippet(offsetExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(offsetExampleCode),
    });
}

}  // namespace

static wxl::FrameworkElement popupPage() {
    return StackPanel {offsetExample()};
}

REGISTER_GALLERY_PAGE(L"Popup", popupPage);
