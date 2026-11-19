// Страница PipsPager -- PipsPagerPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText integratedHeader{"Snippets/PipsPager/IntegratedFlipView.html"};
gallery::LazyLoadedText integratedCode{"Snippets/PipsPager/IntegratedFlipView.h"};

FrameworkElement integrated() {
#include "Snippets/PipsPager/IntegratedFlipView.h"

    return gallery::controlExample({
        .header = gallery::snippet(integratedHeader),
        .example = example,
        .code = gallery::snippet(integratedCode),
    });
}

gallery::LazyLoadedText optionsHeader{"Snippets/PipsPager/Options.html"};
gallery::LazyLoadedText optionsCode{"Snippets/PipsPager/Options.h"};

FrameworkElement options() {
#include "Snippets/PipsPager/Options.h"

    return gallery::controlExample({
        .header = gallery::snippet(optionsHeader),
        .example = pager,
        .options = {options},
        .code = gallery::snippet(optionsCode),
    });
}

}  // namespace

static wxl::FrameworkElement pipsPagerPage() {
    return StackPanel {integrated(), options()};
}

REGISTER_GALLERY_PAGE(L"PipsPager", pipsPagerPage);
