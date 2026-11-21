// Страница ProgressRing -- ProgressRingPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText indeterminateHeader{"Snippets/ProgressRing/IndeterminateProgressRing.html"};
gallery::LazyLoadedText indeterminateCode{"Snippets/ProgressRing/IndeterminateProgressRing.h"};

FrameworkElement indeterminate() {
    using namespace wxl::dsl;
#include "Snippets/ProgressRing/IndeterminateProgressRing.h"

    return gallery::controlExample({
        .header = gallery::snippet(indeterminateHeader),
        .example = ring,
        .options = {activeSwitch},
        .code = gallery::snippet(indeterminateCode),
    });
}

gallery::LazyLoadedText determinateHeader{"Snippets/ProgressRing/DeterminateProgressRing.html"};
gallery::LazyLoadedText determinateCode{"Snippets/ProgressRing/DeterminateProgressRing.h"};

FrameworkElement determinate() {
    using namespace wxl::dsl;
#include "Snippets/ProgressRing/DeterminateProgressRing.h"

    return gallery::controlExample({
        .header = gallery::snippet(determinateHeader),
        .example = ring,
        .options = {progressSlider},
        .code = gallery::snippet(determinateCode),
    });
}

}  // namespace

static wxl::FrameworkElement progressRingPage() {
    return StackPanel {indeterminate(), determinate()};
}

REGISTER_GALLERY_PAGE(L"ProgressRing", progressRingPage);
