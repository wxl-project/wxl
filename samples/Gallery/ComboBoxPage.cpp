// Страница ComboBox — ComboBoxPage оригинала: три примера.
//
// Элементы оригинала — строки и объекты FontItem с шаблоном; здесь это
// ComboBoxItem с подписью, а выбранная строка находится по индексу.

#include "Pages.h"
#include "Shell.h"
#include "ShowDialog.h"

#include <algorithm>
#include <cwchar>
#include <iterator>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText inlineHeader{"Snippets/ComboBox/Inline.html"};
gallery::LazyLoadedText inlineCode{"Snippets/ComboBox/Inline.h"};

FrameworkElement inlineExample() {
#include "Snippets/ComboBox/Inline.h"

    return gallery::controlExample({
        .header = gallery::snippet(inlineHeader),
        .example = StackPanel {combo, output},
        .code = gallery::snippet(inlineCode),
    });
}

gallery::LazyLoadedText sourceHeader{"Snippets/ComboBox/ItemsSource.html"};
gallery::LazyLoadedText sourceCode{"Snippets/ComboBox/ItemsSource.h"};

FrameworkElement sourceExample() {
#include "Snippets/ComboBox/ItemsSource.h"

    return gallery::controlExample({
        .header = gallery::snippet(sourceHeader),
        .example = StackPanel {combo, output},
        .code = gallery::snippet(sourceCode),
    });
}

gallery::LazyLoadedText editableHeader{"Snippets/ComboBox/Editable.html"};
gallery::LazyLoadedText editableCode{"Snippets/ComboBox/Editable.h"};

FrameworkElement editableExample() {
#include "Snippets/ComboBox/Editable.h"

    return gallery::controlExample({
        .header = gallery::snippet(editableHeader),
        .example = StackPanel {combo, output},
        .code = gallery::snippet(editableCode),
    });
}

}  // namespace

static wxl::FrameworkElement comboBoxPage() {
    return StackPanel {inlineExample(), sourceExample(), editableExample()};
}

REGISTER_GALLERY_PAGE(L"ComboBox", comboBoxPage);
