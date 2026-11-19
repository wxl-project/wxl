// Страница GridView -- GridViewPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <ranges>
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include "CustomDataObject.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/GridView/BasicGridviewSimpleDatatemplate.html"};
gallery::LazyLoadedText basicCode{"Snippets/GridView/BasicGridviewSimpleDatatemplate.h"};

FrameworkElement basic() {
#include "Snippets/GridView/BasicGridviewSimpleDatatemplate.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText layoutHeader{"Snippets/GridView/GridviewLayoutCustomization.html"};
gallery::LazyLoadedText layoutCode{"Snippets/GridView/GridviewLayoutCustomization.h"};

FrameworkElement customizedLayout() {
#include "Snippets/GridView/GridviewLayoutCustomization.h"

    return gallery::controlExample({
        .header = gallery::snippet(layoutHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(layoutCode),
    });
}

gallery::LazyLoadedText contentHeader{"Snippets/GridView/ContentInsideGridview.html"};
gallery::LazyLoadedText contentCode{"Snippets/GridView/ContentInsideGridview.h"};

FrameworkElement contentGrid() {
#include "Snippets/GridView/ContentInsideGridview.h"

    return gallery::controlExample({
        .header = gallery::snippet(contentHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(contentCode),
    });
}

}  // namespace

static wxl::FrameworkElement gridViewPage() {
    return StackPanel {basic(), customizedLayout(), contentGrid()};
}

REGISTER_GALLERY_PAGE(L"GridView", gridViewPage);
