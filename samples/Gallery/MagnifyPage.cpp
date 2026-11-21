// Страница эффекта Magnify — перенесена из приложения Effects: описание, примеры и их исходники в духе Gallery.
//
// Код каждого примера — файл в Snippets/Magnify/: он включается по #include в функцию, которая строит пример, и он же,
// вшитый байтами, показывается под примером. Показанное и работающее — один и тот же текст.

#include "Pages.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText ButtonHeader{"Snippets/Magnify/Button.html"};
gallery::LazyLoadedText ButtonCode{"Snippets/Magnify/Button.h"};

FrameworkElement buttonExample() {
    using namespace wxl::dsl;
#include "Snippets/Magnify/Button.h"

    return gallery::controlExample({
        .header = gallery::snippet(ButtonHeader),
        .example = example,
        .code = gallery::snippet(ButtonCode),
    });
}

gallery::LazyLoadedText ToolbarHeader{"Snippets/Magnify/Toolbar.html"};
gallery::LazyLoadedText ToolbarCode{"Snippets/Magnify/Toolbar.h"};

FrameworkElement toolbarExample() {
    using namespace wxl::dsl;
#include "Snippets/Magnify/Toolbar.h"

    return gallery::controlExample({
        .header = gallery::snippet(ToolbarHeader),
        .example = example,
        .code = gallery::snippet(ToolbarCode),
    });
}

gallery::LazyLoadedText CocktailHeader{"Snippets/Magnify/Cocktail.html"};
gallery::LazyLoadedText CocktailCode{"Snippets/Magnify/Cocktail.h"};

FrameworkElement cocktailExample() {
    using namespace wxl::dsl;
#include "Snippets/Magnify/Cocktail.h"

    return gallery::controlExample({
        .header = gallery::snippet(CocktailHeader),
        .example = example,
        .code = gallery::snippet(CocktailCode),
    });
}

}  // namespace

static wxl::FrameworkElement magnifyPage() {
    return StackPanel {buttonExample(), toolbarExample(), cocktailExample()};
}

REGISTER_GALLERY_PAGE(L"Magnify", magnifyPage);
