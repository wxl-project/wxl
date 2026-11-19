// Страница CommandBarFlyout -- CommandBarFlyoutPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText onAppObjectHeader{"Snippets/CommandBarFlyout/OnAppObject.html"};
gallery::LazyLoadedText onAppObjectCode{"Snippets/CommandBarFlyout/OnAppObject.h"};

FrameworkElement onAppObject() {
#include "Snippets/CommandBarFlyout/OnAppObject.h"

    return gallery::controlExample({
        .header = gallery::snippet(onAppObjectHeader),
        .example = example,
        .code = gallery::snippet(onAppObjectCode),
    });
}

}  // namespace

static wxl::FrameworkElement commandBarFlyoutPage() {
    return StackPanel {onAppObject()};
}

REGISTER_GALLERY_PAGE(L"CommandBarFlyout", commandBarFlyoutPage);
