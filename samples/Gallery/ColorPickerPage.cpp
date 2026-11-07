// Страница ColorPicker — ColorPickerPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText propertiesHeader{"Snippets/ColorPicker/Properties.html"};
gallery::LazyLoadedText propertiesCode{"Snippets/ColorPicker/Properties.h"};

FrameworkElement properties() {
#include "Snippets/ColorPicker/Properties.h"

    return gallery::controlExample({
        .header = gallery::snippet(propertiesHeader),
        .example = picker,
        .options = {options},
        .code = gallery::snippet(propertiesCode),
    });
}

}  // namespace

static wxl::FrameworkElement colorPickerPage() {
    return StackPanel {properties()};
}

REGISTER_GALLERY_PAGE(L"ColorPicker", colorPickerPage);
