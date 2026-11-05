// Страница AccessibilityKeyboard — AccessibilityKeyboardPage оригинала: порядок табуляции, стрелки, горячие клавиши
// и клавиши доступа; между разделами пояснения, под ними шесть примеров.

#include "Pages.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Input.h>
#include <wxl/Microsoft.UI.Xaml.Automation.Peers.Enums.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t automaticTabOrderCode[] = {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAutomaticTabOrder.h.embed"
};
constexpr char8_t manualTabOrderCode[] = {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardManualTabOrder.h.embed"
};
constexpr char8_t automaticArrowKeysCode[] = {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAutomaticArrowKeys.h.embed"
};
constexpr char8_t manualArrowKeysCode[] = {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardManualArrowKeys.h.embed"
};
constexpr char8_t acceleratorsCode[] = {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAccelerators.h.embed"
};
constexpr char8_t accessKeysCode[] = {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAccessKeys.h.embed"
};

constexpr char8_t introText[] = {
#include "Snippets/AccessibilityKeyboard/Intro.html.embed"
};
constexpr char8_t tabOrderText[] = {
#include "Snippets/AccessibilityKeyboard/TabOrder.html.embed"
};
constexpr char8_t automaticTabOrderText[] = {
#include "Snippets/AccessibilityKeyboard/AutomaticTabOrder.html.embed"
};
constexpr char8_t manualTabOrderText[] = {
#include "Snippets/AccessibilityKeyboard/ManualTabOrder.html.embed"
};
constexpr char8_t arrowKeysText[] = {
#include "Snippets/AccessibilityKeyboard/ArrowKeys.html.embed"
};
constexpr char8_t automaticArrowKeysText[] = {
#include "Snippets/AccessibilityKeyboard/AutomaticArrowKeys.html.embed"
};
constexpr char8_t manualArrowKeysText[] = {
#include "Snippets/AccessibilityKeyboard/ManualArrowKeys.html.embed"
};
constexpr char8_t keyboardShortcutsText[] = {
#include "Snippets/AccessibilityKeyboard/KeyboardShortcuts.html.embed"
};
constexpr char8_t acceleratorsText[] = {
#include "Snippets/AccessibilityKeyboard/Accelerators.html.embed"
};
constexpr char8_t accessKeysText[] = {
#include "Snippets/AccessibilityKeyboard/AccessKeys.html.embed"
};

// Заголовок раздела: уровень сообщается экранному диктору.
TextBlock section(char16_t const* text, bool second) {
    return TextBlock {Margin {0, second ? 20 : 0, 0, 0}, automationHeadingLevel = AutomationHeadingLevel::Level2, styles.TextBlock.Subtitle, text};
}

TextBlock subsection(char16_t const* text) {
    return TextBlock {Margin {0, 20, 0, 0}, automationHeadingLevel = AutomationHeadingLevel::Level3, styles.TextBlock.BodyStrong, text};
}

FrameworkElement place(wxl::FrameworkElement const& shown, gallery::Snippet code) {
    return gallery::controlExample({.example = shown, .code = code});
}

FrameworkElement automaticTabOrder() {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAutomaticTabOrder.h"
    return place(example, gallery::snippet(automaticTabOrderCode));
}

FrameworkElement manualTabOrder() {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardManualTabOrder.h"
    return place(example, gallery::snippet(manualTabOrderCode));
}

FrameworkElement automaticArrowKeys() {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAutomaticArrowKeys.h"
    return place(example, gallery::snippet(automaticArrowKeysCode));
}

FrameworkElement manualArrowKeys() {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardManualArrowKeys.h"
    return place(example, gallery::snippet(manualArrowKeysCode));
}

FrameworkElement accelerators() {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAccelerators.h"
    return place(example, gallery::snippet(acceleratorsCode));
}

FrameworkElement accessKeys() {
#include "Snippets/AccessibilityKeyboard/AccessibilityKeyboardAccessKeys.h"
    return place(example, gallery::snippet(accessKeysCode));
}

}  // namespace

FrameworkElement gallery::accessibilityKeyboardPage() {
    auto references = StackPanel {automationName = u"Arrow keys references"};
    for (auto const& [title, anchor] : std::initializer_list<std::pair<char16_t const*, char16_t const*>> {
             {u"Keyboard interactions: Navigation", u"#navigation"},
             {u"Keyboard interactions: Home and End keys", u"#home-and-end-keys"},
             {u"Keyboard interactions: Page up and Page down keys", u"#page-up-and-page-down-keys"},
             {u"Keyboard interactions: Control group", u"#control-group"}}) {
        references.children().append(HyperlinkButton {
            content = title, navigateUri = std::u16string {u"https://learn.microsoft.com/windows/apps/design/input/keyboard-interactions"} + anchor});
    }

    return StackPanel {
        spacing = 12.0,
        gallery::prose(gallery::snippet(introText)),

        section(u"Tab order", false),
        gallery::prose(gallery::snippet(tabOrderText)),

        TextBlock {automationHeadingLevel = AutomationHeadingLevel::Level3, styles.TextBlock.BodyStrong, u"Automatic tab order"},
        gallery::prose(gallery::snippet(automaticTabOrderText)),
        automaticTabOrder(),

        TextBlock {automationHeadingLevel = AutomationHeadingLevel::Level3, styles.TextBlock.BodyStrong, u"Manual tab order"},
        gallery::prose(gallery::snippet(manualTabOrderText)),
        manualTabOrder(),

        section(u"Arrow keys", true),
        gallery::prose(gallery::snippet(arrowKeysText)),
        TextBlock {Margin {0, 8, 0, 0}, u"See also:"},
        references,

        subsection(u"Automatically supporting arrow keys"),
        gallery::prose(gallery::snippet(automaticArrowKeysText)),
        automaticArrowKeys(),

        subsection(u"Manually supporting arrow keys with XYFocusKeyboardNavigation"),
        gallery::prose(gallery::snippet(manualArrowKeysText)),
        manualArrowKeys(),

        section(u"Keyboard shortcuts", true),
        gallery::prose(gallery::snippet(keyboardShortcutsText)),

        subsection(u"Accelerators"),
        gallery::prose(gallery::snippet(acceleratorsText)),
        accelerators(),

        subsection(u"Access keys"),
        gallery::prose(gallery::snippet(accessKeysText)),
        accessKeys(),
    };
}
