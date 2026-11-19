// Страница эффекта Glass — перенесена из приложения Effects: описание, примеры и их исходники в духе Gallery.
//
// Код каждого примера — файл в Snippets/Glass/: он включается по #include в функцию, которая строит пример, и он же,
// вшитый байтами, показывается под примером. Показанное и работающее — один и тот же текст.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText FrostHeader{"Snippets/Glass/Frost.html"};
gallery::LazyLoadedText FrostCode{"Snippets/Glass/Frost.h"};

FrameworkElement frostExample() {
#include "Snippets/Glass/Frost.h"

    return gallery::controlExample({
        .header = gallery::snippet(FrostHeader),
        .example = example,
        .code = gallery::snippet(FrostCode),
    });
}

gallery::LazyLoadedText TintedHeader{"Snippets/Glass/Tinted.html"};
gallery::LazyLoadedText TintedCode{"Snippets/Glass/Tinted.h"};

FrameworkElement tintedExample() {
#include "Snippets/Glass/Tinted.h"

    return gallery::controlExample({
        .header = gallery::snippet(TintedHeader),
        .example = example,
        .code = gallery::snippet(TintedCode),
    });
}

}  // namespace

static wxl::FrameworkElement glassPage() {
    return StackPanel {frostExample(), tintedExample()};
}

REGISTER_GALLERY_PAGE(L"Glass", glassPage);
