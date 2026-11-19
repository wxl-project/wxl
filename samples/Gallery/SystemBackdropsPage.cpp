// Страница SystemBackdrops -- SystemBackdropsPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText backdropTypesHeader{"Snippets/SystemBackdrops/SystemBackdropsBackdropTypes.html"};
gallery::LazyLoadedText backdropTypesCode{"Snippets/SystemBackdrops/SystemBackdropsBackdropTypes.h"};

FrameworkElement backdropTypes() {
#include "Snippets/SystemBackdrops/SystemBackdropsBackdropTypes.h"

    return gallery::controlExample({
        .header = gallery::snippet(backdropTypesHeader),
        .example = example,
        .code = gallery::snippet(backdropTypesCode),
    });
}

gallery::LazyLoadedText micaBackdropHeader{"Snippets/SystemBackdrops/SystemBackdropsMicacontroller.html"};
gallery::LazyLoadedText micaBackdropCode{"Snippets/SystemBackdrops/SystemBackdropsMicacontroller.h"};

FrameworkElement micaBackdropExample() {
#include "Snippets/SystemBackdrops/SystemBackdropsMicacontroller.h"

    return gallery::controlExample({
        .header = gallery::snippet(micaBackdropHeader),
        .example = example,
        .code = gallery::snippet(micaBackdropCode),
    });
}

gallery::LazyLoadedText acrylicBackdropHeader{"Snippets/SystemBackdrops/SystemBackdropsDesktopacryliccontroller.html"};
gallery::LazyLoadedText acrylicBackdropCode{"Snippets/SystemBackdrops/SystemBackdropsDesktopacryliccontroller.h"};

FrameworkElement acrylicBackdropExample() {
#include "Snippets/SystemBackdrops/SystemBackdropsDesktopacryliccontroller.h"

    return gallery::controlExample({
        .header = gallery::snippet(acrylicBackdropHeader),
        .example = example,
        .code = gallery::snippet(acrylicBackdropCode),
    });
}

}  // namespace

static wxl::FrameworkElement systemBackdropsPage() {
    return StackPanel {backdropTypes(), micaBackdropExample(), acrylicBackdropExample()};
}

REGISTER_GALLERY_PAGE(L"SystemBackdrops", systemBackdropsPage);
