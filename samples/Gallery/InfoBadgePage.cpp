// Страница InfoBadge -- InfoBadgePage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText embeddedHeader{"Snippets/InfoBadge/EmbeddedNavigationView.html"};
gallery::LazyLoadedText embeddedCode{"Snippets/InfoBadge/EmbeddedNavigationView.h"};

FrameworkElement embedded() {
#include "Snippets/InfoBadge/EmbeddedNavigationView.h"

    return gallery::controlExample({
        .header = gallery::snippet(embeddedHeader),
        .example = Grid {navigation},
        .options = {StackPanel {width = 160, opacity, displayMode}},
        .code = gallery::snippet(embeddedCode),
    });
}

gallery::LazyLoadedText differentHeader{"Snippets/InfoBadge/DifferentInfoBadgeStyles.html"};
gallery::LazyLoadedText differentCode{"Snippets/InfoBadge/DifferentInfoBadgeStyles.h"};

FrameworkElement different() {
#include "Snippets/InfoBadge/DifferentInfoBadgeStyles.h"

    return gallery::controlExample({
        .header = gallery::snippet(differentHeader),
        .example = StackPanel {hAlign.center, spacing = 20.0, orientation.horizontal, iconBadge, valueBadge, dotBadge},
        .options = {StackPanel {width = 160, kind}},
        .code = gallery::snippet(differentCode),
    });
}

gallery::LazyLoadedText insideHeader{"Snippets/InfoBadge/PlacingInfoBadgeInsideAnother.html"};
gallery::LazyLoadedText insideCode{"Snippets/InfoBadge/PlacingInfoBadgeInsideAnother.h"};

FrameworkElement inside() {
    return gallery::controlExample({
        .header = gallery::snippet(insideHeader),
        .example =
#include "Snippets/InfoBadge/PlacingInfoBadgeInsideAnother.h"
        ,
        .code = gallery::snippet(insideCode),
    });
}

gallery::LazyLoadedText dynamicHeader{"Snippets/InfoBadge/InfoBadgeDynamicValue.html"};
gallery::LazyLoadedText dynamicCode{"Snippets/InfoBadge/InfoBadgeDynamicValue.h"};

FrameworkElement dynamic() {
#include "Snippets/InfoBadge/InfoBadgeDynamicValue.h"

    return gallery::controlExample({
        .header = gallery::snippet(dynamicHeader),
        .example = badge,
        .options = {StackPanel {width = 160, valueBox}},
        .code = gallery::snippet(dynamicCode),
    });
}

}  // namespace

static wxl::FrameworkElement infoBadgePage() {
    return StackPanel {embedded(), different(), inside(), dynamic()};
}

REGISTER_GALLERY_PAGE(L"InfoBadge", infoBadgePage);
