// Страница PagerControl -- PagerControlPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/PagerControl/Basic.html"};
gallery::LazyLoadedText basicCode{"Snippets/PagerControl/Basic.h"};

FrameworkElement basic() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example =
#include "Snippets/PagerControl/Basic.h"
        ,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText displayModesHeader{"Snippets/PagerControl/DisplayModes.html"};
gallery::LazyLoadedText displayModesCode{"Snippets/PagerControl/DisplayModes.h"};

FrameworkElement displayModes() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(displayModesHeader),
        .example =
#include "Snippets/PagerControl/DisplayModes.h"
        ,
        .code = gallery::snippet(displayModesCode),
    });
}

gallery::LazyLoadedText unboundedHeader{"Snippets/PagerControl/Unbounded.html"};
gallery::LazyLoadedText unboundedCode{"Snippets/PagerControl/Unbounded.h"};

FrameworkElement unbounded() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(unboundedHeader),
        .example =
#include "Snippets/PagerControl/Unbounded.h"
        ,
        .code = gallery::snippet(unboundedCode),
    });
}

}  // namespace

wxl::FrameworkElement gallery::pagerControlPage() {
    return StackPanel {basic(), displayModes(), unbounded()};
}
