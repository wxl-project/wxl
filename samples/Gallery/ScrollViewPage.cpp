// Страница ScrollView -- ScrollViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Windows.Globalization.NumberFormatting.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText withContentHeader{"Snippets/ScrollView/Content.html"};
gallery::LazyLoadedText withContentCode{"Snippets/ScrollView/Content.h"};

FrameworkElement withContent() {
#include "Snippets/ScrollView/Content.h"

    return gallery::controlExample({
        .header = gallery::snippet(withContentHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(withContentCode),
    });
}

gallery::LazyLoadedText constantVelocityHeader{"Snippets/ScrollView/ConstantVelocity.html"};
gallery::LazyLoadedText constantVelocityCode{"Snippets/ScrollView/ConstantVelocity.h"};

FrameworkElement constantVelocity() {
#include "Snippets/ScrollView/ConstantVelocity.h"

    return gallery::controlExample({
        .header = gallery::snippet(constantVelocityHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(constantVelocityCode),
    });
}

gallery::LazyLoadedText customAnimationHeader{"Snippets/ScrollView/CustomAnimation.html"};
gallery::LazyLoadedText customAnimationCode{"Snippets/ScrollView/CustomAnimation.h"};

FrameworkElement customAnimation() {
#include "Snippets/ScrollView/CustomAnimation.h"

    return gallery::controlExample({
        .header = gallery::snippet(customAnimationHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(customAnimationCode),
    });
}

}  // namespace

static wxl::FrameworkElement scrollViewPage() {
    return StackPanel {withContent(), constantVelocity(), customAnimation()};
}

REGISTER_GALLERY_PAGE(L"ScrollView", scrollViewPage);
