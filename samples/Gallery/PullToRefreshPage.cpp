// Страница PullToRefresh -- PullToRefreshPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "Box.h"
#include <wxl/Windows.Foundation.h>
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/PullToRefresh/BasicPulltorefresh.html"};
gallery::LazyLoadedText basicCode{"Snippets/PullToRefresh/BasicPulltorefresh.h"};

FrameworkElement basic() {
#include "Snippets/PullToRefresh/BasicPulltorefresh.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText customIconHeader{"Snippets/PullToRefresh/CustomIconPulltorefresh.html"};
gallery::LazyLoadedText customIconCode{"Snippets/PullToRefresh/CustomIconPulltorefresh.h"};

FrameworkElement customIcon() {
#include "Snippets/PullToRefresh/CustomIconPulltorefresh.h"

    return gallery::controlExample({
        .header = gallery::snippet(customIconHeader),
        .example = example,
        .code = gallery::snippet(customIconCode),
    });
}

}  // namespace

static wxl::FrameworkElement pullToRefreshPage() {
    return StackPanel {basic(), customIcon()};
}

REGISTER_GALLERY_PAGE(L"PullToRefresh", pullToRefreshPage);
