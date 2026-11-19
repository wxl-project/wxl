// Страница CompactSizing -- CompactSizingPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText controlsHeader{"Snippets/CompactSizing/CompactSizingControls.html"};
gallery::LazyLoadedText controlsCode{"Snippets/CompactSizing/CompactSizingControls.h"};

FrameworkElement controls() {
#include "Snippets/CompactSizing/CompactSizingControls.h"

    return gallery::controlExample({
        .header = gallery::snippet(controlsHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(controlsCode),
    });
}

}  // namespace

static wxl::FrameworkElement compactSizingPage() {
    return StackPanel {
        gallery::prose(
            u8"<p><span style=\"semibold\">Controls that support compact styling:</span></p>"
            u8"<ul>"
            u8"<li>ListView</li>"
            u8"<li>TextBox</li>"
            u8"<li>PasswordBox</li>"
            u8"<li>AutoSuggestBox</li>"
            u8"<li>ComboBox</li>"
            u8"<li>DatePicker</li>"
            u8"<li>TimePicker</li>"
            u8"<li>TreeView</li>"
            u8"<li>NavigationView</li>"
            u8"<li>MenuBar</li>"
            u8"</ul>",
            Thickness {0, 24, 0, 0}),
        controls()};
}

REGISTER_GALLERY_PAGE(L"CompactSizing", compactSizingPage);
