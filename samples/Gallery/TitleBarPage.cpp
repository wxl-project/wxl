// Страница TitleBar -- TitleBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText introText{"Snippets/TitleBar/Intro.html"};

gallery::LazyLoadedText configurationHeader{"Snippets/TitleBar/TitlebarConfiguration.html"};
gallery::LazyLoadedText configurationCode{"Snippets/TitleBar/TitlebarConfiguration.h"};

FrameworkElement configurationExample() {
#include "Snippets/TitleBar/TitlebarConfiguration.h"

    return gallery::controlExample({
        .header = gallery::snippet(configurationHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(configurationCode),
    });
}

gallery::LazyLoadedText dragRegionsHeader{"Snippets/TitleBar/TitlebarDragRegions.html"};
gallery::LazyLoadedText dragRegionsCode{"Snippets/TitleBar/TitlebarDragRegions.h"};

FrameworkElement dragRegions() {
#include "Snippets/TitleBar/TitlebarDragRegions.h"

    return gallery::controlExample({
        .header = gallery::snippet(dragRegionsHeader),
        .example = example,
        .code = gallery::snippet(dragRegionsCode),
    });
}

gallery::LazyLoadedText endToEndHeader{"Snippets/TitleBar/EndEndTitlebarSample.html"};
gallery::LazyLoadedText endToEndCode{"Snippets/TitleBar/EndEndTitlebarSample.h"};

FrameworkElement endToEnd() {
#include "Snippets/TitleBar/EndEndTitlebarSample.h"

    return gallery::controlExample({
        .header = gallery::snippet(endToEndHeader),
        .example = example,
        .code = gallery::snippet(endToEndCode),
    });
}

}  // namespace

static wxl::FrameworkElement titleBarPage() {
    return StackPanel {
        Margin {0, 12, 0, 0},
        gallery::prose(gallery::snippet(introText), Thickness {}), configurationExample(), dragRegions(), endToEnd()};
}

REGISTER_GALLERY_PAGE(L"TitleBar", titleBarPage);
