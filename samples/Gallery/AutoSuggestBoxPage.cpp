// Страница AutoSuggestBox — AutoSuggestBoxPage оригинала: два примера.
//
// Подсказки оригинала — строки и объекты ControlInfoDataItem; здесь везде
// строки (названия), а по выбранному названию контрол находится в каталоге.

#include "Pages.h"
#include "Shell.h"
#include "Box.h"

#include <string>
#include <vector>

using namespace wxl;
using namespace wxl::dsl;

namespace {

#include "Snippets/AutoSuggestBox/Cats.h"

gallery::LazyLoadedText basicHeader{"Snippets/AutoSuggestBox/Basic.html"};
gallery::LazyLoadedText basicCode{"Snippets/AutoSuggestBox/Basic.h"};

FrameworkElement basic() {
#include "Snippets/AutoSuggestBox/Basic.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = StackPanel {orientation.horizontal, box, output},
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText searchHeader{"Snippets/AutoSuggestBox/SearchBox.html"};
gallery::LazyLoadedText searchCode{"Snippets/AutoSuggestBox/SearchBox.h"};

FrameworkElement searchBox() {
#include "Snippets/AutoSuggestBox/SearchBox.h"

    return gallery::controlExample({
        .header = gallery::snippet(searchHeader),
        .example = Grid {RowDefinitions<u"auto,auto">, box, Border {row = 1, details}},
        .code = gallery::snippet(searchCode),
    });
}

}  // namespace

static wxl::FrameworkElement autoSuggestBoxPage() {
    return StackPanel {basic(), searchBox()};
}

REGISTER_GALLERY_PAGE(L"AutoSuggestBox", autoSuggestBoxPage);
