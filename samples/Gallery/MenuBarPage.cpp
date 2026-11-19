// Страница MenuBar -- MenuBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/MenuBar/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/MenuBar/Simple.h"};

FrameworkElement simple() {
#include "Snippets/MenuBar/Simple.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = StackPanel {output, bar},
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText acceleratorsHeader{"Snippets/MenuBar/KeyboardAccelerators.html"};
gallery::LazyLoadedText acceleratorsCode{"Snippets/MenuBar/KeyboardAccelerators.h"};

FrameworkElement accelerators() {
#include "Snippets/MenuBar/KeyboardAccelerators.h"

    return gallery::controlExample({
        .header = gallery::snippet(acceleratorsHeader),
        .example = StackPanel {output, bar},
        .code = gallery::snippet(acceleratorsCode),
    });
}

gallery::LazyLoadedText submenusHeader{"Snippets/MenuBar/Submenus.html"};
gallery::LazyLoadedText submenusCode{"Snippets/MenuBar/Submenus.h"};

FrameworkElement submenus() {
#include "Snippets/MenuBar/Submenus.h"

    return gallery::controlExample({
        .header = gallery::snippet(submenusHeader),
        .example = StackPanel {output, bar},
        .code = gallery::snippet(submenusCode),
    });
}

}  // namespace

static wxl::FrameworkElement menuBarPage() {
    return StackPanel {simple(), accelerators(), submenus()};
}

REGISTER_GALLERY_PAGE(L"MenuBar", menuBarPage);
