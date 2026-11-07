// Страница ToggleSplitButton — ToggleSplitButtonPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "event_awaitable.h"

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText listHeader{"Snippets/ToggleSplitButton/BulletList.html"};
gallery::LazyLoadedText listCode{"Snippets/ToggleSplitButton/BulletList.h"};

FrameworkElement listExample() {
#include "Snippets/ToggleSplitButton/BulletList.h"

    return gallery::controlExample({
        .header = gallery::snippet(listHeader),
        .example = listButton,
        .options = {richBox},
        .code = gallery::snippet(listCode),
    });
}

}  // namespace

static wxl::FrameworkElement toggleSplitButtonPage() {
    return StackPanel {listExample()};
}

REGISTER_GALLERY_PAGE(L"ToggleSplitButton", toggleSplitButtonPage);
