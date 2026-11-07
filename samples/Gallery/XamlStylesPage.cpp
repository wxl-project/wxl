// Страница XamlStyles — XamlStylesPage оригинала. Стиль XAML — набор значений свойств для одного типа; у wxl это
// пресет (Preset): значения, лежащие в переменной. Неявного стиля описание не знает — его роль у функции,
// делающей контрол.

#include "Pages.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t definitionText[] = {
#include "Snippets/XamlStyles/Definition.html.embed"
};
constexpr char8_t styleKindsText[] = {
#include "Snippets/XamlStyles/StyleKinds.html.embed"
};

constexpr char8_t applyingHeader[] = {
#include "Snippets/XamlStyles/XamlStylesCreatingApplyingStyle.html.embed"
};
constexpr char8_t applyingCode[] = {
#include "Snippets/XamlStyles/XamlStylesCreatingApplyingStyle.h.embed"
};
constexpr char8_t implicitHeader[] = {
#include "Snippets/XamlStyles/XamlStylesStyleWithoutKeyImplicit.html.embed"
};
constexpr char8_t implicitCode[] = {
#include "Snippets/XamlStyles/XamlStylesStyleWithoutKeyImplicit.h.embed"
};

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
