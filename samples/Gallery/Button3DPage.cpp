// Страница эффекта Button3D — стенд клавиши: описание, клавиатура и панель
// настроек, которая пишет в сеттеры эффекта.
//
// Код примера — файл в Snippets/Button3D/: он включается по #include в функцию,
// которая строит пример, и он же, вшитый байтами, показывается под примером.
// Показанное и работающее — один и тот же текст.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText KeypadHeader{"Snippets/Button3D/Keypad.html"};
gallery::LazyLoadedText KeypadCode{"Snippets/Button3D/Keypad.h"};

FrameworkElement keypadExample() {
#include "Snippets/Button3D/Keypad.h"

    return gallery::controlExample({
        .header = gallery::snippet(KeypadHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(KeypadCode),
    });
}

}  // namespace

static wxl::FrameworkElement button3DPage() {
    return StackPanel {keypadExample()};
}

REGISTER_GALLERY_PAGE(L"Button3D", button3DPage);
