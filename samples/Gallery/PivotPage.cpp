// Страница Pivot -- PivotPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t basicHeader[] = {
#include "Snippets/Pivot/BasicPivot.html.embed"
};
constexpr char8_t basicCode[] = {
#include "Snippets/Pivot/BasicPivot.h.embed"
};

FrameworkElement basic() {
#include "Snippets/Pivot/BasicPivot.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

}  // namespace

static wxl::FrameworkElement pivotPage() {
    return StackPanel {basic()};
}

REGISTER_GALLERY_PAGE(L"Pivot", pivotPage);
