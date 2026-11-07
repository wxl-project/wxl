// Страница InfoBar -- InfoBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText changeHeader{"Snippets/InfoBar/ClosableInfoBarOptionsChange.html"};
gallery::LazyLoadedText changeCode{"Snippets/InfoBar/ClosableInfoBarOptionsChange.h"};

FrameworkElement change() {
#include "Snippets/InfoBar/ClosableInfoBarOptionsChange.h"

    return gallery::controlExample({
        .header = gallery::snippet(changeHeader),
        .example = bar,
        .options = {StackPanel {width = 150, isOpen, severity}},
        .code = gallery::snippet(changeCode),
    });
}

gallery::LazyLoadedText longShortHeader{"Snippets/InfoBar/ClosableInfoBarLongShort.html"};
gallery::LazyLoadedText longShortCode{"Snippets/InfoBar/ClosableInfoBarLongShort.h"};

FrameworkElement longShort() {
#include "Snippets/InfoBar/ClosableInfoBarLongShort.h"

    return gallery::controlExample({
        .header = gallery::snippet(longShortHeader),
        .example = bar,
        .options = {StackPanel {width = 150, isOpen, length, actionButton}},
        .code = gallery::snippet(longShortCode),
    });
}

gallery::LazyLoadedText displayHeader{"Snippets/InfoBar/ClosableInfoBarOptionsDisplay.html"};
gallery::LazyLoadedText displayCode{"Snippets/InfoBar/ClosableInfoBarOptionsDisplay.h"};

FrameworkElement display() {
#include "Snippets/InfoBar/ClosableInfoBarOptionsDisplay.h"

    return gallery::controlExample({
        .header = gallery::snippet(displayHeader),
        .example = bar,
        .options = {StackPanel {width = 150, isOpen, isIconVisible, isClosable}},
        .code = gallery::snippet(displayCode),
    });
}

}  // namespace

static wxl::FrameworkElement infoBarPage() {
    return StackPanel {change(), longShort(), display()};
}

REGISTER_GALLERY_PAGE(L"InfoBar", infoBarPage);
