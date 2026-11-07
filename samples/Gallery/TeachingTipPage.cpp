// Страница TeachingTip -- TeachingTipPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText targetedHeader{"Snippets/TeachingTip/ShowTargetedTeachingtipButton.html"};
gallery::LazyLoadedText targetedCode{"Snippets/TeachingTip/ShowTargetedTeachingtipButton.h"};

FrameworkElement targeted() {
#include "Snippets/TeachingTip/ShowTargetedTeachingtipButton.h"

    return gallery::controlExample({
        .header = gallery::snippet(targetedHeader),
        .example = example,
        .code = gallery::snippet(targetedCode),
    });
}

gallery::LazyLoadedText nonTargetedHeader{"Snippets/TeachingTip/ShowNonTargetedTeachingtip.html"};
gallery::LazyLoadedText nonTargetedCode{"Snippets/TeachingTip/ShowNonTargetedTeachingtip.h"};

FrameworkElement nonTargeted() {
#include "Snippets/TeachingTip/ShowNonTargetedTeachingtip.h"

    return gallery::controlExample({
        .header = gallery::snippet(nonTargetedHeader),
        .example = example,
        .code = gallery::snippet(nonTargetedCode),
    });
}

gallery::LazyLoadedText heroHeader{"Snippets/TeachingTip/ShowTargetedTeachingtipHero.html"};
gallery::LazyLoadedText heroCode{"Snippets/TeachingTip/ShowTargetedTeachingtipHero.h"};

FrameworkElement hero() {
#include "Snippets/TeachingTip/ShowTargetedTeachingtipHero.h"

    return gallery::controlExample({
        .header = gallery::snippet(heroHeader),
        .example = example,
        .code = gallery::snippet(heroCode),
    });
}

}  // namespace

static wxl::FrameworkElement teachingTipPage() {
    return StackPanel {targeted(), nonTargeted(), hero()};
}

REGISTER_GALLERY_PAGE(L"TeachingTip", teachingTipPage);
