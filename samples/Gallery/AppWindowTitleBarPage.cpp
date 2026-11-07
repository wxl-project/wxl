// Страница AppWindowTitleBar -- AppWindowTitleBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText introText{"Snippets/AppWindowTitleBar/Intro.html"};

gallery::LazyLoadedText colorCustomizationHeader{"Snippets/AppWindowTitleBar/AppwindowtitlebarColorCustomization.html"};
gallery::LazyLoadedText colorCustomizationCode{"Snippets/AppWindowTitleBar/AppwindowtitlebarColorCustomization.h"};

FrameworkElement colorCustomization() {
#include "Snippets/AppWindowTitleBar/AppwindowtitlebarColorCustomization.h"

    return gallery::controlExample({
        .header = gallery::snippet(colorCustomizationHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(colorCustomizationCode),
    });
}

gallery::LazyLoadedText extendHeader{"Snippets/AppWindowTitleBar/ExtendingContentAppwindowtitlebarArea.html"};
gallery::LazyLoadedText extendCode{"Snippets/AppWindowTitleBar/ExtendingContentAppwindowtitlebarArea.h"};

FrameworkElement extend() {
#include "Snippets/AppWindowTitleBar/ExtendingContentAppwindowtitlebarArea.h"

    return gallery::controlExample({
        .header = gallery::snippet(extendHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(extendCode),
    });
}

gallery::LazyLoadedText themeHeightHeader{"Snippets/AppWindowTitleBar/AppwindowtitlebarPreferredThemeHeight.html"};
gallery::LazyLoadedText themeHeightCode{"Snippets/AppWindowTitleBar/AppwindowtitlebarPreferredThemeHeight.h"};

FrameworkElement themeHeight() {
#include "Snippets/AppWindowTitleBar/AppwindowtitlebarPreferredThemeHeight.h"

    return gallery::controlExample({
        .header = gallery::snippet(themeHeightHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(themeHeightCode),
    });
}

}  // namespace

static wxl::FrameworkElement appWindowTitleBarPage() {
    return StackPanel {
        gallery::prose(gallery::snippet(introText), Thickness {0, 8, 0, 0}), colorCustomization(), extend(), themeHeight()};
}

REGISTER_GALLERY_PAGE(L"AppWindowTitleBar", appWindowTitleBarPage);
