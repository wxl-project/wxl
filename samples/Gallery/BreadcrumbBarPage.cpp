// Страница BreadcrumbBar -- BreadcrumbBarPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <string>
#include <vector>
#include "Announce.h"
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/BreadcrumbBar/BreadcrumbbarControl.html"};
gallery::LazyLoadedText basicCode{"Snippets/BreadcrumbBar/BreadcrumbbarControl.h"};

FrameworkElement basic() {
#include "Snippets/BreadcrumbBar/BreadcrumbbarControl.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText customTemplateHeader{"Snippets/BreadcrumbBar/BreadcrumbbarControlCustomDatatemplate.html"};
gallery::LazyLoadedText customTemplateCode{"Snippets/BreadcrumbBar/BreadcrumbbarControlCustomDatatemplate.h"};

FrameworkElement customTemplate() {
#include "Snippets/BreadcrumbBar/BreadcrumbbarControlCustomDatatemplate.h"

    return gallery::controlExample({
        .header = gallery::snippet(customTemplateHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(customTemplateCode),
    });
}

}  // namespace

static wxl::FrameworkElement breadcrumbBarPage() {
    return StackPanel {basic(), customTemplate()};
}

REGISTER_GALLERY_PAGE(L"BreadcrumbBar", breadcrumbBarPage);
