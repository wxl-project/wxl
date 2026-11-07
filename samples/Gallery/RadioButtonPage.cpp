// Страница RadioButton — RadioButtonPage оригинала: два примера.
//
// Строки `x:String` оригинала — элементы RadioButton: коллекция Items держит
// объекты, а строку из неё wxl не собирает.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText groupHeader{"Snippets/RadioButton/Group.html"};
gallery::LazyLoadedText groupCode{"Snippets/RadioButton/Group.h"};

FrameworkElement groupExample() {
#include "Snippets/RadioButton/Group.h"

    return gallery::controlExample({
        .header = gallery::snippet(groupHeader),
        .example = StackPanel {group},
        .output = {output},
        .code = gallery::snippet(groupCode),
    });
}

gallery::LazyLoadedText stringsHeader{"Snippets/RadioButton/Strings.html"};
gallery::LazyLoadedText stringsCode{"Snippets/RadioButton/Strings.h"};

FrameworkElement stringsExample() {
#include "Snippets/RadioButton/Strings.h"

    return gallery::controlExample({
        .header = gallery::snippet(stringsHeader),
        .example = StackPanel {background, border, sample},
        .code = gallery::snippet(stringsCode),
    });
}

}  // namespace

static wxl::FrameworkElement radioButtonPage() {
    return StackPanel {groupExample(), stringsExample()};
}

REGISTER_GALLERY_PAGE(L"RadioButton", radioButtonPage);
