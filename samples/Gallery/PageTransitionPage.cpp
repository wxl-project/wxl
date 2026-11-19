// Страница PageTransition -- PageTransitionPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <memory>
#include "MotionPages.h"
#include "PagedFrame.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.Animation.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText pageTransitionsHeader{"Snippets/PageTransition/PageTransitions.html"};
gallery::LazyLoadedText pageTransitionsCode{"Snippets/PageTransition/PageTransitions.h"};

FrameworkElement pageTransitions() {
#include "Snippets/PageTransition/PageTransitions.h"

    return gallery::controlExample({
        .header = gallery::snippet(pageTransitionsHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(pageTransitionsCode),
    });
}

}  // namespace

static wxl::FrameworkElement pageTransitionPage() {
    return StackPanel {pageTransitions()};
}

REGISTER_GALLERY_PAGE(L"PageTransition", pageTransitionPage);
