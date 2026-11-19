// Страница ToggleSwitch — ToggleSwitchPage оригинала: два примера.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/ToggleSwitch/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/ToggleSwitch/Simple.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/ToggleSwitch/Simple.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText customHeader{"Snippets/ToggleSwitch/Custom.html"};
gallery::LazyLoadedText customCode{"Snippets/ToggleSwitch/Custom.h"};

FrameworkElement custom() {
    return gallery::controlExample({
        .header = gallery::snippet(customHeader),
        .example =
#include "Snippets/ToggleSwitch/Custom.h"
        ,
        .code = gallery::snippet(customCode),
    });
}

}  // namespace

static wxl::FrameworkElement toggleSwitchPage() {
    return StackPanel {simple(), custom()};
}

REGISTER_GALLERY_PAGE(L"ToggleSwitch", toggleSwitchPage);
