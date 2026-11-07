// Страница XamlStyles — XamlStylesPage оригинала. Стиль XAML — набор значений свойств для одного типа; у wxl это
// пресет (Preset): значения, лежащие в переменной. Неявного стиля описание не знает — его роль у функции,
// делающей контрол.

#include "Pages.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText definitionText{"Snippets/XamlStyles/Definition.html"};
gallery::LazyLoadedText styleKindsText{"Snippets/XamlStyles/StyleKinds.html"};

gallery::LazyLoadedText applyingHeader{"Snippets/XamlStyles/XamlStylesCreatingApplyingStyle.html"};
gallery::LazyLoadedText applyingCode{"Snippets/XamlStyles/XamlStylesCreatingApplyingStyle.h"};
gallery::LazyLoadedText implicitHeader{"Snippets/XamlStyles/XamlStylesStyleWithoutKeyImplicit.html"};
gallery::LazyLoadedText implicitCode{"Snippets/XamlStyles/XamlStylesStyleWithoutKeyImplicit.h"};

FrameworkElement applying() {
#include "Snippets/XamlStyles/XamlStylesCreatingApplyingStyle.h"
    return gallery::controlExample({.header = gallery::snippet(applyingHeader), .example = example, .code = gallery::snippet(applyingCode)});
}

FrameworkElement implicit() {
#include "Snippets/XamlStyles/XamlStylesStyleWithoutKeyImplicit.h"
    return gallery::controlExample({.header = gallery::snippet(implicitHeader), .example = example, .code = gallery::snippet(implicitCode)});
}

}  // namespace

static FrameworkElement xamlStylesPage() {
    return StackPanel {
        spacing = 12.0,
        TextBlock {Margin {0, 12, 0, 4}, styles.TextBlock.Subtitle, u"Creating and using styles"},
        gallery::prose(gallery::snippet(definitionText), Thickness {}),
        gallery::prose(gallery::snippet(styleKindsText), Thickness {}),
        applying(),
        implicit(),
    };
}

REGISTER_GALLERY_PAGE(L"XamlStyles", xamlStylesPage);
