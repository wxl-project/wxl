// Страница FlipView -- FlipViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <ranges>
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText declaredHeader{"Snippets/FlipView/SimpleFlipviewItemsDeclared.html"};
gallery::LazyLoadedText declaredCode{"Snippets/FlipView/SimpleFlipviewItemsDeclared.h"};

FrameworkElement declared() {
#include "Snippets/FlipView/SimpleFlipviewItemsDeclared.h"

    return gallery::controlExample({
        .header = gallery::snippet(declaredHeader),
        .example = example,
        .code = gallery::snippet(declaredCode),
    });
}

gallery::LazyLoadedText boundHeader{"Snippets/FlipView/FlipviewShowingBoundData.html"};
gallery::LazyLoadedText boundCode{"Snippets/FlipView/FlipviewShowingBoundData.h"};

FrameworkElement bound() {
#include "Snippets/FlipView/FlipviewShowingBoundData.h"

    return gallery::controlExample({
        .header = gallery::snippet(boundHeader),
        .example = example,
        .code = gallery::snippet(boundCode),
    });
}

gallery::LazyLoadedText verticalHeader{"Snippets/FlipView/VerticalFlipview.html"};
gallery::LazyLoadedText verticalCode{"Snippets/FlipView/VerticalFlipview.h"};

FrameworkElement vertical() {
#include "Snippets/FlipView/VerticalFlipview.h"

    return gallery::controlExample({
        .header = gallery::snippet(verticalHeader),
        .example = example,
        .code = gallery::snippet(verticalCode),
    });
}

}  // namespace

static wxl::FrameworkElement flipViewPage() {
    return StackPanel {declared(), bound(), vertical()};
}

REGISTER_GALLERY_PAGE(L"FlipView", flipViewPage);
