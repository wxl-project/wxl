// Страница XamlCompInterop -- XamlCompInteropPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <wxl/Microsoft.UI.Composition.h>
#include "Resource.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Input.h>
#include <wxl/Microsoft.UI.Xaml.Media.h>
#include <wxl/Microsoft.UI.Xaml.Shapes.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText naturalMotionHeader{"Snippets/XamlCompInterop/XamlCompInteropNaturalMotionCompositionAnimation.html"};
gallery::LazyLoadedText naturalMotionCode{"Snippets/XamlCompInterop/XamlCompInteropNaturalMotionCompositionAnimation.h"};

FrameworkElement naturalMotion() {
#include "Snippets/XamlCompInterop/XamlCompInteropNaturalMotionCompositionAnimation.h"

    return gallery::controlExample({
        .header = gallery::snippet(naturalMotionHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(naturalMotionCode),
    });
}

gallery::LazyLoadedText ellipseHeader{"Snippets/XamlCompInterop/XamlCompInteropExpressionanimationEllipseElement.html"};
gallery::LazyLoadedText ellipseCode{"Snippets/XamlCompInterop/XamlCompInteropExpressionanimationEllipseElement.h"};

FrameworkElement ellipse() {
#include "Snippets/XamlCompInterop/XamlCompInteropExpressionanimationEllipseElement.h"

    return gallery::controlExample({
        .header = gallery::snippet(ellipseHeader),
        .example = example,
        .code = gallery::snippet(ellipseCode),
    });
}

gallery::LazyLoadedText severalHeader{"Snippets/XamlCompInterop/XamlCompInteropDrivingSeveralRelatedAnimations.html"};
gallery::LazyLoadedText severalCode{"Snippets/XamlCompInterop/XamlCompInteropDrivingSeveralRelatedAnimations.h"};

FrameworkElement several() {
#include "Snippets/XamlCompInterop/XamlCompInteropDrivingSeveralRelatedAnimations.h"

    return gallery::controlExample({
        .header = gallery::snippet(severalHeader),
        .example = example,
        .code = gallery::snippet(severalCode),
    });
}

gallery::LazyLoadedText circleHeader{"Snippets/XamlCompInterop/XamlCompInteropReferenceActualsizeExpressionanimationsMake.html"};
gallery::LazyLoadedText circleCode{"Snippets/XamlCompInterop/XamlCompInteropReferenceActualsizeExpressionanimationsMake.h"};

FrameworkElement circle() {
#include "Snippets/XamlCompInterop/XamlCompInteropReferenceActualsizeExpressionanimationsMake.h"

    return gallery::controlExample({
        .header = gallery::snippet(circleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(circleCode),
    });
}

gallery::LazyLoadedText actualOffsetHeader{"Snippets/XamlCompInterop/XamlCompInteropReferenceActualoffsetActualsizeExpressionanimations.html"};
gallery::LazyLoadedText actualOffsetCode{"Snippets/XamlCompInterop/XamlCompInteropReferenceActualoffsetActualsizeExpressionanimations.h"};

FrameworkElement actualOffset() {
#include "Snippets/XamlCompInterop/XamlCompInteropReferenceActualoffsetActualsizeExpressionanimations.h"

    return gallery::controlExample({
        .header = gallery::snippet(actualOffsetHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(actualOffsetCode),
    });
}

}  // namespace

static wxl::FrameworkElement xamlCompInteropPage() {
    return StackPanel {naturalMotion(), ellipse(), several(), circle(), actualOffset()};
}

REGISTER_GALLERY_PAGE(L"XamlCompInterop", xamlCompInteropPage);
