// Страница CustomXamlConditionals — CustomXamlConditionalsPage оригинала. Условие XAML (IXamlCondition) считается
// один раз при разборе разметки; у wxl разметки нет, условие — обычное условие программы, и оно считается заново, когда
// флаг меняется. Два флага оригинала — два переключателя вверху.

#include "Pages.h"
#include "Shell.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

struct Flags {
    core::observable<bool> newExperience {true};
    core::observable<bool> legacyMode {false};
};

gallery::LazyLoadedText elementsHeader{"Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalElements.html"};
gallery::LazyLoadedText elementsCode{"Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalElements.h"};
gallery::LazyLoadedText attributesHeader{"Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalAttributes.html"};
gallery::LazyLoadedText attributesCode{"Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalAttributes.h"};
gallery::LazyLoadedText settersHeader{"Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalSettersStyle.html"};
gallery::LazyLoadedText settersCode{"Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalSettersStyle.h"};

FrameworkElement elements(std::shared_ptr<Flags> const& flags) {
#include "Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalElements.h"
    return gallery::controlExample({.header = gallery::snippet(elementsHeader), .example = example, .code = gallery::snippet(elementsCode)});
}

FrameworkElement attributes(std::shared_ptr<Flags> const& flags) {
#include "Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalAttributes.h"
    return gallery::controlExample({.header = gallery::snippet(attributesHeader), .example = example, .code = gallery::snippet(attributesCode)});
}

FrameworkElement setters(std::shared_ptr<Flags> const& flags) {
#include "Snippets/CustomXamlConditionals/CustomXamlConditionalsConditionalSettersStyle.h"
    return gallery::controlExample({.header = gallery::snippet(settersHeader), .example = example, .code = gallery::snippet(settersCode)});
}

}  // namespace

static FrameworkElement customXamlConditionalsPage() {
    auto const flags = gallery::hold<Flags>();
    return StackPanel {
        spacing = 12.0,
        InfoBar {
            title = u"Evaluated by the program",
            Margin {0, 24, 0, 0},
            isClosable = false,
            isOpen = true,
            severity = InfoBarSeverity::Informational,
            message = u"A XAML condition (IXamlCondition) is evaluated once, when the markup is parsed. Here the conditions are conditions of "
                      u"the program, so the flags below change the examples at once.",
        },
        StackPanel {orientation.horizontal, spacing = 24.0,
                    ToggleSwitch {header = u"NewExperience", isOn = Bind {flags->newExperience}},
                    ToggleSwitch {header = u"LegacyMode", isOn = Bind {flags->legacyMode}}},
        elements(flags),
        attributes(flags),
        setters(flags),
    };
}

REGISTER_GALLERY_PAGE(L"CustomXamlConditionals", customXamlConditionalsPage);
