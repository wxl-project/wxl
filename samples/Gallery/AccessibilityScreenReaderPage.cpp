// Страница AccessibilityScreenReader — AccessibilityScreenReaderPage оригинала: доступные имена, описания,
// ориентиры и заголовки; пояснения и одиннадцать примеров под ними.

#include "Pages.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>
#include <wxl/Microsoft.UI.Xaml.Automation.Peers.Enums.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t nameFromContentCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameFromContent.h.embed"
};
constexpr char8_t nameFromHeaderCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameFromHeader.h.embed"
};
constexpr char8_t nameOnListViewCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameOnListView.h.embed"
};
constexpr char8_t nameOnImageCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameOnImage.h.embed"
};
constexpr char8_t labeledByCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderLabeledBy.h.embed"
};
constexpr char8_t descriptionCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderDescriptionHelpTextAdd.h.embed"
};
constexpr char8_t positionCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderPositionIndicatePositionElement.h.embed"
};
constexpr char8_t rawCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderRemoveControlContentVisual.h.embed"
};
constexpr char8_t landmarksCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderLandmarks.h.embed"
};
constexpr char8_t headingsCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderHeadings.h.embed"
};
constexpr char8_t groupsCode[] = {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderControlGroups.h.embed"
};

constexpr char8_t introText[] = {
#include "Snippets/AccessibilityScreenReader/Intro.html.embed"
};
constexpr char8_t accessibleNamesText[] = {
#include "Snippets/AccessibilityScreenReader/AccessibleNames.html.embed"
};
constexpr char8_t automaticNameText[] = {
#include "Snippets/AccessibilityScreenReader/AutomaticName.html.embed"
};
constexpr char8_t manualNameText[] = {
#include "Snippets/AccessibilityScreenReader/ManualName.html.embed"
};
constexpr char8_t labeledByText[] = {
#include "Snippets/AccessibilityScreenReader/LabeledBy.html.embed"
};
constexpr char8_t commonPropertiesText[] = {
#include "Snippets/AccessibilityScreenReader/CommonProperties.html.embed"
};
constexpr char8_t visualTreeText[] = {
#include "Snippets/AccessibilityScreenReader/VisualTree.html.embed"
};
constexpr char8_t landmarksHeadingsText[] = {
#include "Snippets/AccessibilityScreenReader/LandmarksHeadings.html.embed"
};
constexpr char8_t landmarksText[] = {
#include "Snippets/AccessibilityScreenReader/Landmarks.html.embed"
};
constexpr char8_t headingsText[] = {
#include "Snippets/AccessibilityScreenReader/Headings.html.embed"
};
constexpr char8_t controlGroupsText[] = {
#include "Snippets/AccessibilityScreenReader/ControlGroups.html.embed"
};

// Подзаголовки раздела; уровень сообщается экранному диктору.
TextBlock heading2(char16_t const* text, bool spaced = true) {
    return TextBlock {Margin {0, spaced ? 20 : 0, 0, 0}, automationHeadingLevel = AutomationHeadingLevel::Level2, styles.TextBlock.Subtitle, text};
}

TextBlock heading3(char16_t const* text) {
    return TextBlock {Margin {0, 20, 0, 0}, automationHeadingLevel = AutomationHeadingLevel::Level3, styles.TextBlock.BodyStrong, text};
}

TextBlock bigHeading3(char16_t const* text) {
    return TextBlock {Margin {0, 20, 0, 0}, automationHeadingLevel = AutomationHeadingLevel::Level3, styles.TextBlock.Subtitle, text};
}

FrameworkElement place(FrameworkElement const& shown, gallery::Snippet code) {
    return gallery::controlExample({.example = shown, .code = code});
}

FrameworkElement nameFromContent() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameFromContent.h"
    return place(example, gallery::snippet(nameFromContentCode));
}

FrameworkElement nameFromHeader() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameFromHeader.h"
    return place(example, gallery::snippet(nameFromHeaderCode));
}

FrameworkElement nameOnListView() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameOnListView.h"
    return place(example, gallery::snippet(nameOnListViewCode));
}

FrameworkElement nameOnImage() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameOnImage.h"
    return place(example, gallery::snippet(nameOnImageCode));
}

FrameworkElement labeledBy() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderLabeledBy.h"
    return place(example, gallery::snippet(labeledByCode));
}

FrameworkElement describedControls() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderDescriptionHelpTextAdd.h"
    return place(example, gallery::snippet(descriptionCode));
}

FrameworkElement positioned() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderPositionIndicatePositionElement.h"
    return place(example, gallery::snippet(positionCode));
}

FrameworkElement removedFromTree() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderRemoveControlContentVisual.h"
    return place(example, gallery::snippet(rawCode));
}

FrameworkElement landmarks() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderLandmarks.h"
    return place(example, gallery::snippet(landmarksCode));
}

FrameworkElement headings() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderHeadings.h"
    return place(example, gallery::snippet(headingsCode));
}

FrameworkElement controlGroups() {
#include "Snippets/AccessibilityScreenReader/AccessibilityScreenReaderControlGroups.h"
    return place(example, gallery::snippet(groupsCode));
}

}  // namespace

static FrameworkElement accessibilityScreenReaderPage() {
    return StackPanel {
        spacing = 12.0,
        gallery::prose(gallery::snippet(introText), Thickness {}),

        heading2(u"Accessible names"),
        gallery::prose(gallery::snippet(accessibleNamesText), Thickness {}),

        heading3(u"Getting an accessible name automatically"),
        gallery::prose(gallery::snippet(automaticNameText), Thickness {}),
        nameFromContent(),
        nameFromHeader(),

        heading3(u"Setting an accessible name manually"),
        gallery::prose(gallery::snippet(manualNameText), Thickness {}),
        nameOnListView(),
        nameOnImage(),

        heading3(u"Using another control to provide an accessible name"),
        gallery::prose(gallery::snippet(labeledByText), Thickness {}),
        labeledBy(),

        heading2(u"Common accessibility properties"),
        gallery::prose(gallery::snippet(commonPropertiesText), Thickness {}),
        describedControls(),
        positioned(),

        heading2(u"Visual tree"),
        gallery::prose(gallery::snippet(visualTreeText), Thickness {}),
        removedFromTree(),

        heading2(u"Landmarks and headings"),
        gallery::prose(gallery::snippet(landmarksHeadingsText), Thickness {}),

        bigHeading3(u"Landmarks"),
        gallery::prose(gallery::snippet(landmarksText), Thickness {}),
        landmarks(),

        bigHeading3(u"Headings"),
        gallery::prose(gallery::snippet(headingsText), Thickness {}),
        headings(),

        bigHeading3(u"Associating smaller groups of controls"),
        gallery::prose(gallery::snippet(controlGroupsText), Thickness {}),
        controlGroups(),
    };
}

REGISTER_GALLERY_PAGE(L"AccessibilityScreenReader", accessibilityScreenReaderPage);
