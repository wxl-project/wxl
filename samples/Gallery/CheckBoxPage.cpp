// Страница CheckBox — CheckBoxPage оригинала: два, три состояния и «выбрать всё».

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText twoHeader{"Snippets/CheckBox/TwoState.html"};
gallery::LazyLoadedText twoCode{"Snippets/CheckBox/TwoState.h"};

FrameworkElement twoState() {
#include "Snippets/CheckBox/TwoState.h"

    return gallery::controlExample({
        .header = gallery::snippet(twoHeader),
        .example = StackPanel {orientation.horizontal, box},
        .output = {output},
        .code = gallery::snippet(twoCode),
    });
}

gallery::LazyLoadedText threeHeader{"Snippets/CheckBox/ThreeState.html"};
gallery::LazyLoadedText threeCode{"Snippets/CheckBox/ThreeState.h"};

FrameworkElement threeState() {
#include "Snippets/CheckBox/ThreeState.h"

    return gallery::controlExample({
        .header = gallery::snippet(threeHeader),
        .example = StackPanel {orientation.horizontal, box},
        .output = {output},
        .code = gallery::snippet(threeCode),
    });
}

gallery::LazyLoadedText allHeader{"Snippets/CheckBox/SelectAll.html"};
gallery::LazyLoadedText allCode{"Snippets/CheckBox/SelectAll.h"};

FrameworkElement selectAll() {
#include "Snippets/CheckBox/SelectAll.h"

    return gallery::controlExample({
        .header = gallery::snippet(allHeader),
        .example = options,
        .code = gallery::snippet(allCode),
    });
}

}  // namespace

static wxl::FrameworkElement checkBoxPage() {
    return StackPanel {twoState(), threeState(), selectAll()};
}

REGISTER_GALLERY_PAGE(L"CheckBox", checkBoxPage);
