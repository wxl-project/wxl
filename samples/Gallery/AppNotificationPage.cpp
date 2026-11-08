// Страница AppNotification -- AppNotificationPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "Application.h"
#include <wxl/Microsoft.Windows.AppNotifications.h>
#include <wxl/Microsoft.Windows.AppNotifications.Builder.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/AppNotification/AppNotificationBasicNotification.html"};
gallery::LazyLoadedText basicCode{"Snippets/AppNotification/AppNotificationBasicNotification.h"};

FrameworkElement basic() {
#include "Snippets/AppNotification/AppNotificationBasicNotification.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText logoHeader{"Snippets/AppNotification/AppNotificationInformationalNotificationLogoCustom.html"};
gallery::LazyLoadedText logoCode{"Snippets/AppNotification/AppNotificationInformationalNotificationLogoCustom.h"};

FrameworkElement customLogo() {
#include "Snippets/AppNotification/AppNotificationInformationalNotificationLogoCustom.h"

    return gallery::controlExample({
        .header = gallery::snippet(logoHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(logoCode),
    });
}

gallery::LazyLoadedText heroHeader{"Snippets/AppNotification/AppNotificationVisualNotificationHeroImage.html"};
gallery::LazyLoadedText heroCode{"Snippets/AppNotification/AppNotificationVisualNotificationHeroImage.h"};

FrameworkElement hero() {
#include "Snippets/AppNotification/AppNotificationVisualNotificationHeroImage.h"

    return gallery::controlExample({
        .header = gallery::snippet(heroHeader),
        .example = example,
        .code = gallery::snippet(heroCode),
    });
}

gallery::LazyLoadedText controlsHeader{"Snippets/AppNotification/NotificationAppnotificationControls.html"};
gallery::LazyLoadedText controlsCode{"Snippets/AppNotification/NotificationAppnotificationControls.h"};

FrameworkElement controls() {
#include "Snippets/AppNotification/NotificationAppnotificationControls.h"

    return gallery::controlExample({
        .header = gallery::snippet(controlsHeader),
        .example = example,
        .code = gallery::snippet(controlsCode),
    });
}

gallery::LazyLoadedText progressHeader{"Snippets/AppNotification/AppNotificationNotificationProgressbar.html"};
gallery::LazyLoadedText progressCode{"Snippets/AppNotification/AppNotificationNotificationProgressbar.h"};

FrameworkElement progress() {
#include "Snippets/AppNotification/AppNotificationNotificationProgressbar.h"

    return gallery::controlExample({
        .header = gallery::snippet(progressHeader),
        .example = example,
        .code = gallery::snippet(progressCode),
    });
}

}  // namespace

static wxl::FrameworkElement appNotificationPage() {
    return StackPanel {
        // Заставка страницы оригинала: подсказка про «Do Not Disturb» и предупреждение о шуме. Отступ prose {0,12,12,12} —
        // дословно отступ RichTextBlock оригинала: prose кладёт HtmlBlock ровно в Content InfoBar, где стоял RichTextBlock.
        StackPanel {
            spacing = 8,
            Margin {0, 16, 0, 0},
            InfoBar {
                isOpen = true,
                severity = InfoBarSeverity::Informational,
                isIconVisible = true,
                isClosable = false,
                content = gallery::prose(
                    u8"If <span style=\"semibold\">Do Not Disturb</span> mode is enabled in system settings, these "
                    u8"notifications will not appear as toast popups, but they will still be added to the notification "
                    u8"center and can be viewed there.",
                    Thickness {0, 12, 12, 12},
                    false),
            },
            InfoBar {
                isOpen = true,
                severity = InfoBarSeverity::Warning,
                title = u"App notifications should not be noisy",
                message = u"App notifications are designed to convey timely and relevant information without disrupting the user "
                          u"experience. Excessive use of sound, prolonged durations, or overly attention-seeking visuals can lead to "
                          u"user fatigue and diminish the overall effectiveness of notifications.",
                isIconVisible = true,
                isClosable = false,
            },
        },
        basic(), customLogo(), hero(), controls(), progress(),
    };
}

REGISTER_GALLERY_PAGE(L"AppNotification", appNotificationPage);
