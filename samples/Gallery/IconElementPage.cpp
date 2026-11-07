// Страница IconElement -- IconElementPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText bitmapIconHeader{"Snippets/IconElement/MulticolorBitmap.html"};
gallery::LazyLoadedText bitmapIconCode{"Snippets/IconElement/MulticolorBitmap.h"};

FrameworkElement bitmapIcon() {
#include "Snippets/IconElement/MulticolorBitmap.h"

    return gallery::controlExample({
        .header = gallery::snippet(bitmapIconHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(bitmapIconCode),
    });
}

gallery::LazyLoadedText fontIconHeader{"Snippets/IconElement/FontGlyph.html"};
gallery::LazyLoadedText fontIconCode{"Snippets/IconElement/FontGlyph.h"};

FrameworkElement fontIcon() {
#include "Snippets/IconElement/FontGlyph.h"

    return gallery::controlExample({
        .header = gallery::snippet(fontIconHeader),
        .example = example,
        .code = gallery::snippet(fontIconCode),
    });
}

gallery::LazyLoadedText imageIconHeader{"Snippets/IconElement/ImageBitmap.html"};
gallery::LazyLoadedText imageIconCode{"Snippets/IconElement/ImageBitmap.h"};

FrameworkElement imageIcon() {
#include "Snippets/IconElement/ImageBitmap.h"

    return gallery::controlExample({
        .header = gallery::snippet(imageIconHeader),
        .example = example,
        .code = gallery::snippet(imageIconCode),
    });
}

gallery::LazyLoadedText svgIconHeader{"Snippets/IconElement/ImageSvg.html"};
gallery::LazyLoadedText svgIconCode{"Snippets/IconElement/ImageSvg.h"};

FrameworkElement svgIcon() {
    return gallery::controlExample({
        .header = gallery::snippet(svgIconHeader),
        .example =
#include "Snippets/IconElement/ImageSvg.h"
        ,
        .code = gallery::snippet(svgIconCode),
    });
}

gallery::LazyLoadedText pathIconHeader{"Snippets/IconElement/Path.html"};
gallery::LazyLoadedText pathIconCode{"Snippets/IconElement/Path.h"};

FrameworkElement pathIcon() {
#include "Snippets/IconElement/Path.h"

    return gallery::controlExample({
        .header = gallery::snippet(pathIconHeader),
        .example = example,
        .code = gallery::snippet(pathIconCode),
    });
}

gallery::LazyLoadedText symbolIconHeader{"Snippets/IconElement/Symbol.html"};
gallery::LazyLoadedText symbolIconCode{"Snippets/IconElement/Symbol.h"};

FrameworkElement symbolIcon() {
#include "Snippets/IconElement/Symbol.h"

    return gallery::controlExample({
        .header = gallery::snippet(symbolIconHeader),
        .example = example,
        .code = gallery::snippet(symbolIconCode),
    });
}

}  // namespace

static wxl::FrameworkElement iconElementPage() {
    return StackPanel {bitmapIcon(), fontIcon(), imageIcon(), svgIcon(), pathIcon(), symbolIcon()};
}

REGISTER_GALLERY_PAGE(L"IconElement", iconElementPage);
