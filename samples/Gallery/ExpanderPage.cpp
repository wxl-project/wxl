// Страница Expander — ExpanderPage оригинала: два примера.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText textHeader{"Snippets/Expander/ExpanderTextHeaderContent.html"};
gallery::LazyLoadedText textCode{"Snippets/Expander/ExpanderTextHeaderContent.h"};

FrameworkElement textHeaderContent() {
    using namespace wxl::dsl;
#include "Snippets/Expander/ExpanderTextHeaderContent.h"

    return gallery::controlExample({
        .header = gallery::snippet(textHeader),
        .example = expander,
        .options = {directionBox},
        .code = gallery::snippet(textCode),
    });
}

gallery::LazyLoadedText alignmentHeader{"Snippets/Expander/ModifyingExpandersContentAlignment.html"};
gallery::LazyLoadedText alignmentCode{"Snippets/Expander/ModifyingExpandersContentAlignment.h"};

FrameworkElement contentAlignmentExample() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(alignmentHeader),
        .example =
#include "Snippets/Expander/ModifyingExpandersContentAlignment.h"
        ,
        .code = gallery::snippet(alignmentCode),
    });
}

}  // namespace

static wxl::FrameworkElement expanderPage() {
    return StackPanel {textHeaderContent(), contentAlignmentExample()};
}

REGISTER_GALLERY_PAGE(L"Expander", expanderPage);
