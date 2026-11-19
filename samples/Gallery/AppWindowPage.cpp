// Страница AppWindow -- AppWindowPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "WindowNative.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText creatingHeader{"Snippets/AppWindow/CreatingCustomizingAppwindowWindow.html"};
gallery::LazyLoadedText creatingCode{"Snippets/AppWindow/CreatingCustomizingAppwindowWindow.h"};

FrameworkElement creating() {
#include "Snippets/AppWindow/CreatingCustomizingAppwindowWindow.h"

    return gallery::controlExample({
        .header = gallery::snippet(creatingHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(creatingCode),
    });
}

gallery::LazyLoadedText centeringHeader{"Snippets/AppWindow/CenteringAppwindowScreenAvailable.html"};
gallery::LazyLoadedText centeringCode{"Snippets/AppWindow/CenteringAppwindowScreenAvailable.h"};

FrameworkElement centering() {
#include "Snippets/AppWindow/CenteringAppwindowScreenAvailable.h"

    return gallery::controlExample({
        .header = gallery::snippet(centeringHeader),
        .example = example,
        .code = gallery::snippet(centeringCode),
    });
}

gallery::LazyLoadedText overlappedHeader{"Snippets/AppWindow/AppwindowOverlapedpresenter.html"};
gallery::LazyLoadedText overlappedCode{"Snippets/AppWindow/AppwindowOverlapedpresenter.h"};

FrameworkElement overlapped() {
#include "Snippets/AppWindow/AppwindowOverlapedpresenter.h"

    return gallery::controlExample({
        .header = gallery::snippet(overlappedHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(overlappedCode),
    });
}

gallery::LazyLoadedText minMaxHeader{"Snippets/AppWindow/AppWindowSettingMinimumMaximumWidth.html"};
gallery::LazyLoadedText minMaxCode{"Snippets/AppWindow/AppWindowSettingMinimumMaximumWidth.h"};

FrameworkElement minMax() {
#include "Snippets/AppWindow/AppWindowSettingMinimumMaximumWidth.h"

    return gallery::controlExample({
        .header = gallery::snippet(minMaxHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(minMaxCode),
    });
}

gallery::LazyLoadedText modalHeader{"Snippets/AppWindow/ModalWindowOverlappedpresenterAppwindow.html"};
gallery::LazyLoadedText modalCode{"Snippets/AppWindow/ModalWindowOverlappedpresenterAppwindow.h"};

FrameworkElement modal() {
#include "Snippets/AppWindow/ModalWindowOverlappedpresenterAppwindow.h"

    return gallery::controlExample({
        .header = gallery::snippet(modalHeader),
        .example = example,
        .code = gallery::snippet(modalCode),
    });
}

gallery::LazyLoadedText fullscreenHeader{"Snippets/AppWindow/AppwindowFullscreenpresenter.html"};
gallery::LazyLoadedText fullscreenCode{"Snippets/AppWindow/AppwindowFullscreenpresenter.h"};

FrameworkElement fullscreen() {
#include "Snippets/AppWindow/AppwindowFullscreenpresenter.h"

    return gallery::controlExample({
        .header = gallery::snippet(fullscreenHeader),
        .example = example,
        .code = gallery::snippet(fullscreenCode),
    });
}

gallery::LazyLoadedText compactOverlayHeader{"Snippets/AppWindow/AppwindowCompactoverlaypresenter.html"};
gallery::LazyLoadedText compactOverlayCode{"Snippets/AppWindow/AppwindowCompactoverlaypresenter.h"};

FrameworkElement compactOverlay() {
#include "Snippets/AppWindow/AppwindowCompactoverlaypresenter.h"

    return gallery::controlExample({
        .header = gallery::snippet(compactOverlayHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(compactOverlayCode),
    });
}

}  // namespace

static wxl::FrameworkElement appWindowPage() {
    return StackPanel {
        TextBlock {Margin {0, 16, 0, -16}, styles.TextBlock.Subtitle, u"General usage of AppWindow"},
        creating(),
        centering(),
        TextBlock {Margin {0, 24, 0, -16}, styles.TextBlock.Subtitle, u"AppWindow Presenters"},
        overlapped(),
        minMax(),
        modal(),
        fullscreen(),
        compactOverlay(),
    };
}

REGISTER_GALLERY_PAGE(L"AppWindow", appWindowPage);
