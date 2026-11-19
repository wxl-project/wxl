// Страница Line -- LinePage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText lineHeader{"Snippets/Line/Line.html"};
gallery::LazyLoadedText lineCode{"Snippets/Line/Line.h"};

FrameworkElement line() {
#include "Snippets/Line/Line.h"

    return gallery::controlExample({
        .header = gallery::snippet(lineHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(lineCode),
    });
}

gallery::LazyLoadedText polylineHeader{"Snippets/Line/Polyline.html"};
gallery::LazyLoadedText polylineCode{"Snippets/Line/Polyline.h"};

FrameworkElement polyline() {
#include "Snippets/Line/Polyline.h"

    return gallery::controlExample({
        .header = gallery::snippet(polylineHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(polylineCode),
    });
}

gallery::LazyLoadedText pathHeader{"Snippets/Line/LinePath.html"};
gallery::LazyLoadedText pathCode{"Snippets/Line/LinePath.h"};

FrameworkElement path() {
#include "Snippets/Line/LinePath.h"

    return gallery::controlExample({
        .header = gallery::snippet(pathHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(pathCode),
    });
}

gallery::LazyLoadedText geometryGroupHeader{"Snippets/Line/LineGeometrygroup.html"};
gallery::LazyLoadedText geometryGroupCode{"Snippets/Line/LineGeometrygroup.h"};

FrameworkElement geometryGroup() {
#include "Snippets/Line/LineGeometrygroup.h"

    return gallery::controlExample({
        .header = gallery::snippet(geometryGroupHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(geometryGroupCode),
    });
}

}  // namespace

static wxl::FrameworkElement linePage() {
    return StackPanel {line(), polyline(), path(), geometryGroup()};
}

REGISTER_GALLERY_PAGE(L"Line", linePage);
