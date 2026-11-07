// Страница AccessibilityScreenReader — AccessibilityScreenReaderPage оригинала: доступные имена, описания,
// ориентиры и заголовки; пояснения и одиннадцать примеров под ними.

#include "Pages.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>
#include <wxl/Microsoft.UI.Xaml.Automation.Peers.Enums.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText nameFromContentCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameFromContent.h"};
gallery::LazyLoadedText nameFromHeaderCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameFromHeader.h"};
gallery::LazyLoadedText nameOnListViewCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameOnListView.h"};
gallery::LazyLoadedText nameOnImageCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderNameOnImage.h"};
gallery::LazyLoadedText labeledByCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderLabeledBy.h"};
gallery::LazyLoadedText descriptionCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderDescriptionHelpTextAdd.h"};
gallery::LazyLoadedText positionCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderPositionIndicatePositionElement.h"};
gallery::LazyLoadedText rawCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderRemoveControlContentVisual.h"};
gallery::LazyLoadedText landmarksCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderLandmarks.h"};
gallery::LazyLoadedText headingsCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderHeadings.h"};
gallery::LazyLoadedText groupsCode{"Snippets/AccessibilityScreenReader/AccessibilityScreenReaderControlGroups.h"};

gallery::LazyLoadedText introText{"Snippets/AccessibilityScreenReader/Intro.html"};
gallery::LazyLoadedText accessibleNamesText{"Snippets/AccessibilityScreenReader/AccessibleNames.html"};
gallery::LazyLoadedText automaticNameText{"Snippets/AccessibilityScreenReader/AutomaticName.html"};
gallery::LazyLoadedText manualNameText{"Snippets/AccessibilityScreenReader/ManualName.html"};
gallery::LazyLoadedText labeledByText{"Snippets/AccessibilityScreenReader/LabeledBy.html"};
gallery::LazyLoadedText commonPropertiesText{"Snippets/AccessibilityScreenReader/CommonProperties.html"};
gallery::LazyLoadedText visualTreeText{"Snippets/AccessibilityScreenReader/VisualTree.html"};
gallery::LazyLoadedText landmarksHeadingsText{"Snippets/AccessibilityScreenReader/LandmarksHeadings.html"};
gallery::LazyLoadedText landmarksText{"Snippets/AccessibilityScreenReader/Landmarks.html"};
gallery::LazyLoadedText headingsText{"Snippets/AccessibilityScreenReader/Headings.html"};
gallery::LazyLoadedText controlGroupsText{"Snippets/AccessibilityScreenReader/ControlGroups.html"};

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
