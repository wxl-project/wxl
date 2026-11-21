// Страница Button — ButtonPage оригинала (Samples/Button): четыре примера,
// те же, что там. Код каждого — файл в Snippets/Button/ (см. Pages.h).

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/Button/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/Button/Simple.h"};

FrameworkElement simple() {
    using namespace wxl::dsl;
#include "Snippets/Button/Simple.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = StackPanel {orientation.horizontal, spacing = 8.0, button, disabled},
        .output = {output},
        .options = {disable},
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText withImageHeader{"Snippets/Button/WithImage.html"};
gallery::LazyLoadedText withImageCode{"Snippets/Button/WithImage.h"};

FrameworkElement withImage() {
    using namespace wxl::dsl;
#include "Snippets/Button/WithImage.h"

    return gallery::controlExample({
        .header = gallery::snippet(withImageHeader),
        .example = StackPanel {orientation.horizontal, button},
        .output = {output},
        .code = gallery::snippet(withImageCode),
    });
}

gallery::LazyLoadedText stylesHeader{"Snippets/Button/BuiltInStyles.html"};
gallery::LazyLoadedText stylesCode{"Snippets/Button/BuiltInStyles.h"};

FrameworkElement builtInStyles() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(stylesHeader),
        .example =
#include "Snippets/Button/BuiltInStyles.h"
        ,
        .code = gallery::snippet(stylesCode),
    });
}

gallery::LazyLoadedText wrappingHeader{"Snippets/Button/Wrapping.html"};
gallery::LazyLoadedText wrappingCode{"Snippets/Button/Wrapping.h"};

FrameworkElement wrapping() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(wrappingHeader),
        .example =
#include "Snippets/Button/Wrapping.h"
        ,
        .code = gallery::snippet(wrappingCode),
    });
}

}  // namespace

static wxl::FrameworkElement buttonPage() {
    return StackPanel {simple(), withImage(), builtInStyles(), wrapping()};
}

REGISTER_GALLERY_PAGE(L"Button", buttonPage);
