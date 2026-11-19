// Страница BadgeNotificationManager -- BadgeNotificationManagerPage оригинала.

#include "Pages.h"
#include "Application.h"
#include <wxl/Microsoft.Windows.BadgeNotifications.h>
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText countHeader{"Snippets/BadgeNotificationManager/BadgeNotificationManagerSettingBadgeNotificationsCount.html"};
gallery::LazyLoadedText countCode{"Snippets/BadgeNotificationManager/BadgeNotificationManagerSettingBadgeNotificationsCount.h"};

FrameworkElement count() {
#include "Snippets/BadgeNotificationManager/BadgeNotificationManagerSettingBadgeNotificationsCount.h"

    return gallery::controlExample({
        .header = gallery::snippet(countHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(countCode),
    });
}

gallery::LazyLoadedText glyphExampleHeader{"Snippets/BadgeNotificationManager/BadgeNotificationManagerSettingBadgeNotificationsGlyph.html"};
gallery::LazyLoadedText glyphExampleCode{"Snippets/BadgeNotificationManager/BadgeNotificationManagerSettingBadgeNotificationsGlyph.h"};

FrameworkElement glyphExample() {
#include "Snippets/BadgeNotificationManager/BadgeNotificationManagerSettingBadgeNotificationsGlyph.h"

    return gallery::controlExample({
        .header = gallery::snippet(glyphExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(glyphExampleCode),
    });
}

}  // namespace

static wxl::FrameworkElement badgeNotificationManagerPage() {
    return StackPanel {
        InfoBar {
            Margin {0, 10, 0, 0},
            isOpen = true,
            isClosable = false,
            severity = InfoBarSeverity::Warning,
            title = u"BadgeNotificationManager is not available in unpackaged mode.",
            message = u"This API requires the app to be running in packaged mode (MSIX).",
        },count(), glyphExample()};
}

REGISTER_GALLERY_PAGE(L"BadgeNotificationManager", badgeNotificationManagerPage);
