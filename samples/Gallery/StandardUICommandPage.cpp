// Страница StandardUICommand -- StandardUICommandPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText exposingHeader{"Snippets/StandardUICommand/ExposingCommand.html"};
gallery::LazyLoadedText exposingCode{"Snippets/StandardUICommand/ExposingCommand.h"};

FrameworkElement exposing() {
#include "Snippets/StandardUICommand/ExposingCommand.h"

    return gallery::controlExample({
        .header = gallery::snippet(exposingHeader),
        .example = example,
        .code = gallery::snippet(exposingCode),
    });
}

}  // namespace

static wxl::FrameworkElement standardUICommandPage() {
    return StackPanel {exposing()};
}

REGISTER_GALLERY_PAGE(L"StandardUICommand", standardUICommandPage);
