// Страница Shape -- ShapePage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText ellipseHeader{"Snippets/Shape/Ellipse.html"};
gallery::LazyLoadedText ellipseCode{"Snippets/Shape/Ellipse.h"};

FrameworkElement ellipse() {
#include "Snippets/Shape/Ellipse.h"

    return gallery::controlExample({
        .header = gallery::snippet(ellipseHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(ellipseCode),
    });
}

gallery::LazyLoadedText rectangleHeader{"Snippets/Shape/Rectangle.html"};
gallery::LazyLoadedText rectangleCode{"Snippets/Shape/Rectangle.h"};

FrameworkElement rectangle() {
#include "Snippets/Shape/Rectangle.h"

    return gallery::controlExample({
        .header = gallery::snippet(rectangleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(rectangleCode),
    });
}

gallery::LazyLoadedText polygonHeader{"Snippets/Shape/Polygon.html"};
gallery::LazyLoadedText polygonCode{"Snippets/Shape/Polygon.h"};

FrameworkElement polygon() {
#include "Snippets/Shape/Polygon.h"

    return gallery::controlExample({
        .header = gallery::snippet(polygonHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(polygonCode),
    });
}

}  // namespace

static wxl::FrameworkElement shapePage() {
    return StackPanel {ellipse(), rectangle(), polygon()};
}

REGISTER_GALLERY_PAGE(L"Shape", shapePage);
