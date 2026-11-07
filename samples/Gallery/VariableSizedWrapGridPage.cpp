// Страница VariableSizedWrapGrid — VariableSizedWrapGridPage оригинала: один пример.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText controlHeader{"Snippets/VariableSizedWrapGrid/VariableSizedWrapGridControl.html"};
gallery::LazyLoadedText controlCode{"Snippets/VariableSizedWrapGrid/VariableSizedWrapGridControl.h"};

FrameworkElement control() {
#include "Snippets/VariableSizedWrapGrid/VariableSizedWrapGridControl.h"

    return gallery::controlExample({
        .header = gallery::snippet(controlHeader),
        .example = grid,
        .options = {orientationGroup},
        .code = gallery::snippet(controlCode),
    });
}

}  // namespace

static wxl::FrameworkElement variableSizedWrapGridPage() {
    return StackPanel {control()};
}

REGISTER_GALLERY_PAGE(L"VariableSizedWrapGrid", variableSizedWrapGridPage);
