// Страница Pivot -- PivotPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/Pivot/BasicPivot.html"};
gallery::LazyLoadedText basicCode{"Snippets/Pivot/BasicPivot.h"};

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
