// Страница ParallaxView -- ParallaxViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "MotionPages.h"
#include "Box.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Shapes.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText listViewHeader{"Snippets/ParallaxView/ParallaxViewParallaxListview.html"};
gallery::LazyLoadedText listViewCode{"Snippets/ParallaxView/ParallaxViewParallaxListview.h"};

FrameworkElement listView() {
#include "Snippets/ParallaxView/ParallaxViewParallaxListview.h"

    return gallery::controlExample({
        .header = gallery::snippet(listViewHeader),
        .example = example,
        .code = gallery::snippet(listViewCode),
    });
}

gallery::LazyLoadedText scrollViewHeader{"Snippets/ParallaxView/ParallaxViewParallaxScrollview.html"};
gallery::LazyLoadedText scrollViewCode{"Snippets/ParallaxView/ParallaxViewParallaxScrollview.h"};

FrameworkElement scrollView() {
#include "Snippets/ParallaxView/ParallaxViewParallaxScrollview.h"

    return gallery::controlExample({
        .header = gallery::snippet(scrollViewHeader),
        .example = example,
        .code = gallery::snippet(scrollViewCode),
    });
}

}  // namespace

static wxl::FrameworkElement parallaxViewPage() {
    return StackPanel {listView(), scrollView()};
}

REGISTER_GALLERY_PAGE(L"ParallaxView", parallaxViewPage);
