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

constexpr char8_t introText[] = {
#include "Snippets/EasingFunction/Intro.html.embed"
};

constexpr char8_t standardHeader[] = {
#include "Snippets/EasingFunction/StandardEasingFunction.html.embed"
};
constexpr char8_t standardCode[] = {
#include "Snippets/EasingFunction/StandardEasingFunction.h.embed"
};

FrameworkElement standard() {
#include "Snippets/EasingFunction/StandardEasingFunction.h"

    return gallery::controlExample({
        .header = gallery::snippet(standardHeader),
        .example = example,
        .code = gallery::snippet(standardCode),
    });
}

constexpr char8_t accelerateHeader[] = {
#include "Snippets/EasingFunction/AccelerateEasingFunction.html.embed"
};
constexpr char8_t accelerateCode[] = {
#include "Snippets/EasingFunction/AccelerateEasingFunction.h.embed"
};

FrameworkElement accelerate() {
#include "Snippets/EasingFunction/AccelerateEasingFunction.h"

    return gallery::controlExample({
        .header = gallery::snippet(accelerateHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(accelerateCode),
    });
}

constexpr char8_t decelerateHeader[] = {
#include "Snippets/EasingFunction/DecelerateEasingFunction.html.embed"
};
constexpr char8_t decelerateCode[] = {
#include "Snippets/EasingFunction/DecelerateEasingFunction.h.embed"
};

FrameworkElement decelerate() {
#include "Snippets/EasingFunction/DecelerateEasingFunction.h"

    return gallery::controlExample({
        .header = gallery::snippet(decelerateHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(decelerateCode),
    });
}

constexpr char8_t otherHeader[] = {
#include "Snippets/EasingFunction/OtherXamlEasingFunctions.html.embed"
};
constexpr char8_t otherCode[] = {
#include "Snippets/EasingFunction/OtherXamlEasingFunctions.h.embed"
};

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
