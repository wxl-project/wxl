// Страница AppWindowTitleBar -- AppWindowTitleBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t introText[] = {
#include "Snippets/AppWindowTitleBar/Intro.html.embed"
};

constexpr char8_t colorCustomizationHeader[] = {
#include "Snippets/AppWindowTitleBar/AppwindowtitlebarColorCustomization.html.embed"
};
constexpr char8_t colorCustomizationCode[] = {
#include "Snippets/AppWindowTitleBar/AppwindowtitlebarColorCustomization.h.embed"
};

FrameworkElement colorCustomization() {
#include "Snippets/AppWindowTitleBar/AppwindowtitlebarColorCustomization.h"

    return gallery::controlExample({
        .header = gallery::snippet(colorCustomizationHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(colorCustomizationCode),
    });
}

constexpr char8_t extendHeader[] = {
#include "Snippets/AppWindowTitleBar/ExtendingContentAppwindowtitlebarArea.html.embed"
};
constexpr char8_t extendCode[] = {
#include "Snippets/AppWindowTitleBar/ExtendingContentAppwindowtitlebarArea.h.embed"
};

FrameworkElement extend() {
#include "Snippets/AppWindowTitleBar/ExtendingContentAppwindowtitlebarArea.h"

    return gallery::controlExample({
        .header = gallery::snippet(extendHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(extendCode),
    });
}

constexpr char8_t themeHeightHeader[] = {
#include "Snippets/AppWindowTitleBar/AppwindowtitlebarPreferredThemeHeight.html.embed"
};
constexpr char8_t themeHeightCode[] = {
#include "Snippets/AppWindowTitleBar/AppwindowtitlebarPreferredThemeHeight.h.embed"
};

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
