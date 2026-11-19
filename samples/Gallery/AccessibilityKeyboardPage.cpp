// Страница AccessibilityKeyboard — AccessibilityKeyboardPage оригинала: порядок табуляции, стрелки, горячие клавиши
// и клавиши доступа; между разделами пояснения, под ними шесть примеров.

#include "Pages.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Input.h>
#include <wxl/Microsoft.UI.Xaml.Automation.Peers.Enums.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText automaticTabOrderCode{"Snippets/AccessibilityKeyboard/AccessibilityKeyboardAutomaticTabOrder.h"};
gallery::LazyLoadedText manualTabOrderCode{"Snippets/AccessibilityKeyboard/AccessibilityKeyboardManualTabOrder.h"};
gallery::LazyLoadedText automaticArrowKeysCode{"Snippets/AccessibilityKeyboard/AccessibilityKeyboardAutomaticArrowKeys.h"};
gallery::LazyLoadedText manualArrowKeysCode{"Snippets/AccessibilityKeyboard/AccessibilityKeyboardManualArrowKeys.h"};
gallery::LazyLoadedText acceleratorsCode{"Snippets/AccessibilityKeyboard/AccessibilityKeyboardAccelerators.h"};
gallery::LazyLoadedText accessKeysCode{"Snippets/AccessibilityKeyboard/AccessibilityKeyboardAccessKeys.h"};

gallery::LazyLoadedText introText{"Snippets/AccessibilityKeyboard/Intro.html"};
gallery::LazyLoadedText tabOrderText{"Snippets/AccessibilityKeyboard/TabOrder.html"};
gallery::LazyLoadedText automaticTabOrderText{"Snippets/AccessibilityKeyboard/AutomaticTabOrder.html"};
gallery::LazyLoadedText manualTabOrderText{"Snippets/AccessibilityKeyboard/ManualTabOrder.html"};
gallery::LazyLoadedText arrowKeysText{"Snippets/AccessibilityKeyboard/ArrowKeys.html"};
gallery::LazyLoadedText automaticArrowKeysText{"Snippets/AccessibilityKeyboard/AutomaticArrowKeys.html"};
gallery::LazyLoadedText manualArrowKeysText{"Snippets/AccessibilityKeyboard/ManualArrowKeys.html"};
gallery::LazyLoadedText keyboardShortcutsText{"Snippets/AccessibilityKeyboard/KeyboardShortcuts.html"};
gallery::LazyLoadedText acceleratorsText{"Snippets/AccessibilityKeyboard/Accelerators.html"};
gallery::LazyLoadedText accessKeysText{"Snippets/AccessibilityKeyboard/AccessKeys.html"};

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

static FrameworkElement accessibilityKeyboardPage() {
    auto references = ItemsControl {automationName = u"Arrow keys references"};
    for (auto const& [title, anchor] : std::initializer_list<std::pair<char16_t const*, char16_t const*>> {
             {u"Keyboard interactions: Navigation", u"#navigation"},
             {u"Keyboard interactions: Home and End keys", u"#home-and-end-keys"},
             {u"Keyboard interactions: Page up and Page down keys", u"#page-up-and-page-down-keys"},
             {u"Keyboard interactions: Control group", u"#control-group"}}) {
        references.items().append(HyperlinkButton {
            content = title, navigateUri = std::u16string {u"https://learn.microsoft.com/windows/apps/design/input/keyboard-interactions"} + anchor});
    }

    return StackPanel {
        spacing = 12.0,
        gallery::prose(gallery::snippet(introText), Thickness {}),

        section(u"Tab order", false),
        gallery::prose(gallery::snippet(tabOrderText), Thickness {}),

        TextBlock {automationHeadingLevel = AutomationHeadingLevel::Level3, styles.TextBlock.BodyStrong, u"Automatic tab order"},
        gallery::prose(gallery::snippet(automaticTabOrderText), Thickness {}),
        automaticTabOrder(),

        TextBlock {automationHeadingLevel = AutomationHeadingLevel::Level3, styles.TextBlock.BodyStrong, u"Manual tab order"},
        gallery::prose(gallery::snippet(manualTabOrderText), Thickness {}),
        manualTabOrder(),

        section(u"Arrow keys", true),
        gallery::prose(gallery::snippet(arrowKeysText), Thickness {}),
        TextBlock {Margin {0, 8, 0, 0}, u"See also:"},
        references,

        subsection(u"Automatically supporting arrow keys"),
        gallery::prose(gallery::snippet(automaticArrowKeysText), Thickness {}),
        automaticArrowKeys(),

        subsection(u"Manually supporting arrow keys with XYFocusKeyboardNavigation"),
        gallery::prose(gallery::snippet(manualArrowKeysText), Thickness {}),
        manualArrowKeys(),

        section(u"Keyboard shortcuts", true),
        gallery::prose(gallery::snippet(keyboardShortcutsText), Thickness {}),

        subsection(u"Accelerators"),
        gallery::prose(gallery::snippet(acceleratorsText), Thickness {}),
        accelerators(),

        subsection(u"Access keys"),
        gallery::prose(gallery::snippet(accessKeysText), Thickness {}),
        accessKeys(),
    };
}

REGISTER_GALLERY_PAGE(L"AccessibilityKeyboard", accessibilityKeyboardPage);
