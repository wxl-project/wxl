// Страница ProgressBar -- ProgressBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText indeterminateHeader{"Snippets/ProgressBar/IndeterminateProgressBar.html"};
gallery::LazyLoadedText indeterminateCode{"Snippets/ProgressBar/IndeterminateProgressBar.h"};

FrameworkElement indeterminate() {
#include "Snippets/ProgressBar/IndeterminateProgressBar.h"

    return gallery::controlExample({
        .header = gallery::snippet(indeterminateHeader),
        .example = bar,
        .options = {stateGroup},
        .code = gallery::snippet(indeterminateCode),
    });
}

gallery::LazyLoadedText determinateHeader{"Snippets/ProgressBar/DeterminateProgressBar.html"};
gallery::LazyLoadedText determinateCode{"Snippets/ProgressBar/DeterminateProgressBar.h"};

FrameworkElement determinate() {
#include "Snippets/ProgressBar/DeterminateProgressBar.h"

    return gallery::controlExample({
        .header = gallery::snippet(determinateHeader),
        .example = row,
        .code = gallery::snippet(determinateCode),
    });
}

}  // namespace

static wxl::FrameworkElement progressBarPage() {
    return StackPanel {indeterminate(), determinate()};
}

REGISTER_GALLERY_PAGE(L"ProgressBar", progressBarPage);
