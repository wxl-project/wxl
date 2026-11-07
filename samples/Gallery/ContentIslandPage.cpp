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

constexpr char8_t basicHeader[] = {
#include "Snippets/ContentIsland/BasicContentIslandContent.html.embed"
};
constexpr char8_t basicCode[] = {
#include "Snippets/ContentIsland/BasicContentIslandContent.h.embed"
};

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
