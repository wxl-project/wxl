// Страница SettingsCard Community Toolkit: контрол wxl (см. decisions/0465), примеры и их исходники.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/SettingsCard/SettingsCardBasic.html"};
gallery::LazyLoadedText basicCode{"Snippets/SettingsCard/SettingsCardBasic.h"};

FrameworkElement basicExample() {
#include "Snippets/SettingsCard/SettingsCardBasic.h"

    return gallery::controlExample({.header = gallery::snippet(basicHeader), .example = example, .code = gallery::snippet(basicCode)});
}

gallery::LazyLoadedText clickableHeader{"Snippets/SettingsCard/SettingsCardClickable.html"};
gallery::LazyLoadedText clickableCode{"Snippets/SettingsCard/SettingsCardClickable.h"};

FrameworkElement clickableExample() {
#include "Snippets/SettingsCard/SettingsCardClickable.h"

    return gallery::controlExample({.header = gallery::snippet(clickableHeader), .example = example, .code = gallery::snippet(clickableCode)});
}

gallery::LazyLoadedText alignmentHeader{"Snippets/SettingsCard/SettingsCardAlignment.html"};
gallery::LazyLoadedText alignmentCode{"Snippets/SettingsCard/SettingsCardAlignment.h"};

FrameworkElement alignmentExample() {
#include "Snippets/SettingsCard/SettingsCardAlignment.h"

    return gallery::controlExample({.header = gallery::snippet(alignmentHeader), .example = example, .code = gallery::snippet(alignmentCode)});
}

}  // namespace

static wxl::FrameworkElement settingsCardPage() {
    return StackPanel {basicExample(), clickableExample(), alignmentExample()};
}

REGISTER_GALLERY_PAGE(L"SettingsCard", settingsCardPage);
