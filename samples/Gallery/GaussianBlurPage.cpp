// Страница эффекта GaussianBlur — перенесена из приложения Effects: описание, примеры и их исходники в духе Gallery.
//
// Код каждого примера — файл в Snippets/GaussianBlur/: он включается по #include в функцию, которая строит пример, и он же,
// вшитый байтами, показывается под примером. Показанное и работающее — один и тот же текст.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText KeycapHeader{"Snippets/GaussianBlur/Keycap.html"};
gallery::LazyLoadedText KeycapCode{"Snippets/GaussianBlur/Keycap.h"};

FrameworkElement keycapExample() {
#include "Snippets/GaussianBlur/Keycap.h"

    return gallery::controlExample({
        .header = gallery::snippet(KeycapHeader),
        .example = example,
        .code = gallery::snippet(KeycapCode),
    });
}

gallery::LazyLoadedText CompareHeader{"Snippets/GaussianBlur/Compare.html"};
gallery::LazyLoadedText CompareCode{"Snippets/GaussianBlur/Compare.h"};

FrameworkElement compareExample() {
#include "Snippets/GaussianBlur/Compare.h"

    return gallery::controlExample({
        .header = gallery::snippet(CompareHeader),
        .example = example,
        .code = gallery::snippet(CompareCode),
    });
}

gallery::LazyLoadedText GammaHeader{"Snippets/GaussianBlur/Gamma.html"};
gallery::LazyLoadedText GammaCode{"Snippets/GaussianBlur/Gamma.h"};

FrameworkElement gammaExample() {
#include "Snippets/GaussianBlur/Gamma.h"

    return gallery::controlExample({
        .header = gallery::snippet(GammaHeader),
        .example = example,
        .code = gallery::snippet(GammaCode),
    });
}

gallery::LazyLoadedText OnTopHeader{"Snippets/GaussianBlur/OnTop.html"};
gallery::LazyLoadedText OnTopCode{"Snippets/GaussianBlur/OnTop.h"};

FrameworkElement onTopExample() {
#include "Snippets/GaussianBlur/OnTop.h"

    return gallery::controlExample({
        .header = gallery::snippet(OnTopHeader),
        .example = example,
        .code = gallery::snippet(OnTopCode),
    });
}

gallery::LazyLoadedText NeonSignHeader{"Snippets/GaussianBlur/NeonSign.html"};
gallery::LazyLoadedText NeonSignCode{"Snippets/GaussianBlur/NeonSign.h"};

FrameworkElement neonSignExample() {
#include "Snippets/GaussianBlur/NeonSign.h"

    return gallery::controlExample({
        .header = gallery::snippet(NeonSignHeader),
        .example = example,
        .code = gallery::snippet(NeonSignCode),
    });
}

}  // namespace

static wxl::FrameworkElement gaussianBlurPage() {
    return StackPanel {keycapExample(), compareExample(), gammaExample(), onTopExample(), neonSignExample()};
}

REGISTER_GALLERY_PAGE(L"GaussianBlur", gaussianBlurPage);
