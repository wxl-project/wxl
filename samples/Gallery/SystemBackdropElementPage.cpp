// Страница SystemBackdropElement -- SystemBackdropElementPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText sampleHeader{"Snippets/SystemBackdropElement/SystembackdropelementSample.html"};
gallery::LazyLoadedText sampleCode{"Snippets/SystemBackdropElement/SystembackdropelementSample.h"};

FrameworkElement sample() {
#include "Snippets/SystemBackdropElement/SystembackdropelementSample.h"

    return gallery::controlExample({
        .header = gallery::snippet(sampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(sampleCode),
    });
}

}  // namespace

static wxl::FrameworkElement systemBackdropElementPage() {
    return StackPanel {
        sample(),
        TextBlock {
            Margin {0, 12, 0, 0},
            textWrapping = TextWrapping::WrapWholeWords,
            u"Theme updates for SystemBackdropElement are triggered by app or OS theme changes, not by setting theme "
            u"directly on the element or a parent control.",
        },
    };
}

REGISTER_GALLERY_PAGE(L"SystemBackdropElement", systemBackdropElementPage);
