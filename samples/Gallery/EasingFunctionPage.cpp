// Страница EasingFunction -- EasingFunctionPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <vector>
#include "Resource.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.h>
#include <wxl/Microsoft.UI.Xaml.Media.Animation.h>
#include <wxl/Microsoft.UI.Xaml.Shapes.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText introText{"Snippets/EasingFunction/Intro.html"};

gallery::LazyLoadedText standardHeader{"Snippets/EasingFunction/StandardEasingFunction.html"};
gallery::LazyLoadedText standardCode{"Snippets/EasingFunction/StandardEasingFunction.h"};

FrameworkElement standard() {
#include "Snippets/EasingFunction/StandardEasingFunction.h"

    return gallery::controlExample({
        .header = gallery::snippet(standardHeader),
        .example = example,
        .code = gallery::snippet(standardCode),
    });
}

gallery::LazyLoadedText accelerateHeader{"Snippets/EasingFunction/AccelerateEasingFunction.html"};
gallery::LazyLoadedText accelerateCode{"Snippets/EasingFunction/AccelerateEasingFunction.h"};

FrameworkElement accelerate() {
#include "Snippets/EasingFunction/AccelerateEasingFunction.h"

    return gallery::controlExample({
        .header = gallery::snippet(accelerateHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(accelerateCode),
    });
}

gallery::LazyLoadedText decelerateHeader{"Snippets/EasingFunction/DecelerateEasingFunction.html"};
gallery::LazyLoadedText decelerateCode{"Snippets/EasingFunction/DecelerateEasingFunction.h"};

FrameworkElement decelerate() {
#include "Snippets/EasingFunction/DecelerateEasingFunction.h"

    return gallery::controlExample({
        .header = gallery::snippet(decelerateHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(decelerateCode),
    });
}

gallery::LazyLoadedText otherHeader{"Snippets/EasingFunction/OtherXamlEasingFunctions.html"};
gallery::LazyLoadedText otherCode{"Snippets/EasingFunction/OtherXamlEasingFunctions.h"};

FrameworkElement other() {
#include "Snippets/EasingFunction/OtherXamlEasingFunctions.h"

    return gallery::controlExample({
        .header = gallery::snippet(otherHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(otherCode),
    });
}

}  // namespace

static wxl::FrameworkElement easingFunctionPage() {
    return StackPanel {gallery::prose(gallery::snippet(introText), Thickness {}), standard(), accelerate(), decelerate(), other()};
}

REGISTER_GALLERY_PAGE(L"EasingFunction", easingFunctionPage);
