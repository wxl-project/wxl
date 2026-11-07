// Страница RichEditBox — RichEditBoxPage оригинала.
//
// Не перенесено: пример с CommandBarFlyout («Share» в меню выделения) —
// нужны SelectionFlyout, ContextFlyout и PrimaryCommands, а коллекция
// ICommandBarElement ещё не проецируется; кнопки «Open» и «Save» примера
// с редактором — им нужны выбор файла и потоки WinRT.

#include "Pages.h"
#include "Shell.h"
#include "event_awaitable.h"
#include <wxl/Microsoft.UI.Xaml.Shapes.h>

#include <limits>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/RichEditBox/SimpleTextEditor.html"};
gallery::LazyLoadedText simpleCode{"Snippets/RichEditBox/SimpleTextEditor.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/RichEditBox/SimpleTextEditor.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText customHeader{"Snippets/RichEditBox/CustomEditor.html"};
gallery::LazyLoadedText customCode{"Snippets/RichEditBox/CustomEditor.h"};

FrameworkElement customEditor() {
#include "Snippets/RichEditBox/CustomEditor.h"

    return gallery::controlExample({
        .header = gallery::snippet(customHeader),
        .example = Grid {RowDefinitions<u"auto,*,auto">, toolbar, Border {row = 1, editor}, Border {row = 2, findBar}},
        .code = gallery::snippet(customCode),
    });
}

gallery::LazyLoadedText mathHeader{"Snippets/RichEditBox/Math.html"};
gallery::LazyLoadedText mathCode{"Snippets/RichEditBox/Math.h"};

FrameworkElement math() {
#include "Snippets/RichEditBox/Math.h"

    return gallery::controlExample({
        .header = gallery::snippet(mathHeader),
        .example = StackPanel {spacing = 8.0, description, editor},
        .code = gallery::snippet(mathCode),
    });
}

gallery::LazyLoadedText mathmlHeader{"Snippets/RichEditBox/WorkingMathml.html"};
gallery::LazyLoadedText mathmlCode{"Snippets/RichEditBox/WorkingMathml.h"};

FrameworkElement workingMathml() {
#include "Snippets/RichEditBox/WorkingMathml.h"

    return gallery::controlExample({
        .header = gallery::snippet(mathmlHeader),
        .example = StackPanel {
            spacing = 16.0,
            editor,
            TextBlock {u"MathML Code", FontWeight {600}, Margin {0, 0, 0, -8}},
            Border {
                CornerRadius {4},
                background = brushes.Card.BackgroundFillColor.Default,
                Padding {8, 8, 8, 0},
                ScrollViewer {maxHeight = 450, content = mathml},
            },
        },
        .options = {setFormula},
        .code = gallery::snippet(mathmlCode),
    });
}

}  // namespace

static wxl::FrameworkElement richEditBoxPage() {
    return StackPanel {simple(), customEditor(), math(), workingMathml()};
}

REGISTER_GALLERY_PAGE(L"RichEditBox", richEditBoxPage);
