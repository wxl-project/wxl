// Страница эффекта Bevel — перенесена из приложения Effects: описание, примеры и их исходники в духе Gallery.
//
// Код каждого примера — файл в Snippets/Bevel/: он включается по #include в функцию, которая строит пример, и он же,
// вшитый байтами, показывается под примером. Показанное и работающее — один и тот же текст.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText CalcPanelHeader{"Snippets/Bevel/CalcPanel.html"};
gallery::LazyLoadedText CalcPanelCode{"Snippets/Bevel/CalcPanel.h"};

FrameworkElement calcPanelExample() {
#include "Snippets/Bevel/CalcPanel.h"

    return gallery::controlExample({
        .header = gallery::snippet(CalcPanelHeader),
        .example = example,
        .code = gallery::snippet(CalcPanelCode),
    });
}

gallery::LazyLoadedText StretchHeader{"Snippets/Bevel/Stretch.html"};
gallery::LazyLoadedText StretchCode{"Snippets/Bevel/Stretch.h"};

FrameworkElement stretchExample() {
#include "Snippets/Bevel/Stretch.h"

    return gallery::controlExample({
        .header = gallery::snippet(StretchHeader),
        .example = example,
        .code = gallery::snippet(StretchCode),
    });
}

gallery::LazyLoadedText KeypadHeader{"Snippets/Bevel/Keypad.html"};
gallery::LazyLoadedText KeypadCode{"Snippets/Bevel/Keypad.h"};

FrameworkElement keypadExample() {
#include "Snippets/Bevel/Keypad.h"

    return gallery::controlExample({
        .header = gallery::snippet(KeypadHeader),
        .example = example,
        .code = gallery::snippet(KeypadCode),
    });
}

}  // namespace

static wxl::FrameworkElement bevelPage() {
    return StackPanel {calcPanelExample(), stretchExample(), keypadExample()};
}

REGISTER_GALLERY_PAGE(L"Bevel", bevelPage);
