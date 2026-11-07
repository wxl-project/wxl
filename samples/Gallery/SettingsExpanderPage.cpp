// Страница SettingsExpander Community Toolkit: контрол wxl (см. decisions/0465), примеры и их исходники.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/SettingsExpander/SettingsExpanderBasic.html"};
gallery::LazyLoadedText basicCode{"Snippets/SettingsExpander/SettingsExpanderBasic.h"};

FrameworkElement basicExample() {
#include "Snippets/SettingsExpander/SettingsExpanderBasic.h"

    return gallery::controlExample({.header = gallery::snippet(basicHeader), .example = example, .code = gallery::snippet(basicCode)});
}

}  // namespace

static wxl::FrameworkElement settingsExpanderPage() {
    return StackPanel {basicExample()};
}

REGISTER_GALLERY_PAGE(L"SettingsExpander", settingsExpanderPage);
