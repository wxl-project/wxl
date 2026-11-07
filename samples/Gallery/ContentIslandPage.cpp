// Страница ContentIsland -- ContentIslandPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Content.h>
#include <wxl/Microsoft.UI.Composition.h>
#include <wxl/Microsoft.UI.Xaml.Hosting.h>
#include <wxl/Microsoft.UI.Xaml.Shapes.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/ContentIsland/BasicContentIslandContent.html"};
gallery::LazyLoadedText basicCode{"Snippets/ContentIsland/BasicContentIslandContent.h"};

FrameworkElement basic() {
#include "Snippets/ContentIsland/BasicContentIslandContent.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

}  // namespace

static wxl::FrameworkElement contentIslandPage() {
    return StackPanel {basic()};
}

REGISTER_GALLERY_PAGE(L"ContentIsland", contentIslandPage);
