// Страница SplitButton — SplitButtonPage оригинала: два примера.
//
// Флайут оригинала — GridView с ItemsWrapGrid в ItemsPanelTemplate; шаблон
// панели здесь не построить, и три колонки даёт ширина GridView.

#include "Pages.h"
#include "Shell.h"
#include "event_awaitable.h"

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText colorHeader{"Snippets/SplitButton/ColorPicker.html"};
gallery::LazyLoadedText colorCode{"Snippets/SplitButton/ColorPicker.h"};

FrameworkElement colorExample() {
#include "Snippets/SplitButton/ColorPicker.h"

    return gallery::controlExample({
        .header = gallery::snippet(colorHeader),
        .example = Grid {columnSpacing = 24.0, colorButton},
        .options = {richBox},
        .code = gallery::snippet(colorCode),
    });
}

gallery::LazyLoadedText textHeader{"Snippets/SplitButton/Text.html"};
gallery::LazyLoadedText textCode{"Snippets/SplitButton/Text.h"};

FrameworkElement textExample() {
#include "Snippets/SplitButton/Text.h"

    return gallery::controlExample({
        .header = gallery::snippet(textHeader),
        .example = chooser,
        .code = gallery::snippet(textCode),
    });
}

}  // namespace

static wxl::FrameworkElement splitButtonPage() {
    return StackPanel {colorExample(), textExample()};
}

REGISTER_GALLERY_PAGE(L"SplitButton", splitButtonPage);
