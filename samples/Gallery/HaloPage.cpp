// Страница эффекта Halo — перенесена из приложения Effects: описание, примеры и их исходники в духе Gallery.
//
// Код каждого примера — файл в Snippets/Halo/: он включается по #include в функцию, которая строит пример, и он же,
// вшитый байтами, показывается под примером. Показанное и работающее — один и тот же текст.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText LcdHeader{"Snippets/Halo/Lcd.html"};
gallery::LazyLoadedText LcdCode{"Snippets/Halo/Lcd.h"};

FrameworkElement lcdExample() {
#include "Snippets/Halo/Lcd.h"

    return gallery::controlExample({
        .header = gallery::snippet(LcdHeader),
        .example = example,
        .code = gallery::snippet(LcdCode),
    });
}

gallery::LazyLoadedText LedHeader{"Snippets/Halo/Led.html"};
gallery::LazyLoadedText LedCode{"Snippets/Halo/Led.h"};

FrameworkElement ledExample() {
#include "Snippets/Halo/Led.h"

    return gallery::controlExample({
        .header = gallery::snippet(LedHeader),
        .example = example,
        .code = gallery::snippet(LedCode),
    });
}

gallery::LazyLoadedText GlowHeader{"Snippets/Halo/Glow.html"};
gallery::LazyLoadedText GlowCode{"Snippets/Halo/Glow.h"};

FrameworkElement glowExample() {
#include "Snippets/Halo/Glow.h"

    return gallery::controlExample({
        .header = gallery::snippet(GlowHeader),
        .example = example,
        .code = gallery::snippet(GlowCode),
    });
}

}  // namespace

static wxl::FrameworkElement haloPage() {
    return StackPanel {lcdExample(), ledExample(), glowExample()};
}

REGISTER_GALLERY_PAGE(L"Halo", haloPage);
