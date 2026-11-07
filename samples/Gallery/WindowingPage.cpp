// Страница Windowing -- WindowingPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText createWindowHeader{"Snippets/Windowing/CreateMultipleWindows.html"};
gallery::LazyLoadedText createWindowCode{"Snippets/Windowing/CreateMultipleWindows.h"};

FrameworkElement createWindow() {
#include "Snippets/Windowing/CreateMultipleWindows.h"

    return gallery::controlExample({
        .header = gallery::snippet(createWindowHeader),
        .example = example,
        .code = gallery::snippet(createWindowCode),
    });
}

gallery::LazyLoadedText sizingHeader{"Snippets/Windowing/WindowSizing.html"};
gallery::LazyLoadedText sizingCode{"Snippets/Windowing/WindowSizing.h"};

FrameworkElement sizing() {
#include "Snippets/Windowing/WindowSizing.h"

    return gallery::controlExample({
        .header = gallery::snippet(sizingHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(sizingCode),
    });
}

}  // namespace

static wxl::FrameworkElement windowingPage() {
    return StackPanel {
        TextBlock {
            Margin {0, 0, 0, 8},
            textWrapping = TextWrapping::Wrap,
            u"Create top-level XAML windows and configure their client-area size. Each example opens a separate window.",
        },createWindow(), sizing()};
}

REGISTER_GALLERY_PAGE(L"Windowing", windowingPage);
