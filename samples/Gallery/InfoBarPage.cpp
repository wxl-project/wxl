// Страница InfoBar -- InfoBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText changeHeader{"Snippets/InfoBar/ClosableInfoBarOptionsChange.html"};
gallery::LazyLoadedText changeCode{"Snippets/InfoBar/ClosableInfoBarOptionsChange.h"};

FrameworkElement change() {
    using namespace wxl::dsl;
#include "Snippets/InfoBar/ClosableInfoBarOptionsChange.h"

    return gallery::controlExample({
        .header = gallery::snippet(changeHeader),
        .example = bar,
        .options = {StackPanel {width = 150, isOpenBox, severityBox}},
        .code = gallery::snippet(changeCode),
    });
}

gallery::LazyLoadedText longShortHeader{"Snippets/InfoBar/ClosableInfoBarLongShort.html"};
gallery::LazyLoadedText longShortCode{"Snippets/InfoBar/ClosableInfoBarLongShort.h"};

FrameworkElement longShort() {
    using namespace wxl::dsl;
#include "Snippets/InfoBar/ClosableInfoBarLongShort.h"

    return gallery::controlExample({
        .header = gallery::snippet(longShortHeader),
        .example = bar,
        .options = {StackPanel {width = 150, isOpenBox, length, actionBox}},
        .code = gallery::snippet(longShortCode),
    });
}

gallery::LazyLoadedText displayHeader{"Snippets/InfoBar/ClosableInfoBarOptionsDisplay.html"};
gallery::LazyLoadedText displayCode{"Snippets/InfoBar/ClosableInfoBarOptionsDisplay.h"};

FrameworkElement display() {
    using namespace wxl::dsl;
#include "Snippets/InfoBar/ClosableInfoBarOptionsDisplay.h"

    return gallery::controlExample({
        .header = gallery::snippet(displayHeader),
        .example = bar,
        .options = {StackPanel {width = 150, isOpenBox, isIconVisibleBox, isClosableBox}},
        .code = gallery::snippet(displayCode),
    });
}

}  // namespace

static wxl::FrameworkElement infoBarPage() {
    return StackPanel {change(), longShort(), display()};
}

REGISTER_GALLERY_PAGE(L"InfoBar", infoBarPage);
