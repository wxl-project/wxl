// Страница TitleBar -- TitleBarPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t introText[] = {
#include "Snippets/TitleBar/Intro.html.embed"
};

constexpr char8_t configurationHeader[] = {
#include "Snippets/TitleBar/TitlebarConfiguration.html.embed"
};
constexpr char8_t configurationCode[] = {
#include "Snippets/TitleBar/TitlebarConfiguration.h.embed"
};

FrameworkElement configurationExample() {
#include "Snippets/TitleBar/TitlebarConfiguration.h"

    return gallery::controlExample({
        .header = gallery::snippet(configurationHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(configurationCode),
    });
}

constexpr char8_t dragRegionsHeader[] = {
#include "Snippets/TitleBar/TitlebarDragRegions.html.embed"
};
constexpr char8_t dragRegionsCode[] = {
#include "Snippets/TitleBar/TitlebarDragRegions.h.embed"
};

FrameworkElement dragRegions() {
#include "Snippets/TitleBar/TitlebarDragRegions.h"

    return gallery::controlExample({
        .header = gallery::snippet(dragRegionsHeader),
        .example = example,
        .code = gallery::snippet(dragRegionsCode),
    });
}

constexpr char8_t endToEndHeader[] = {
#include "Snippets/TitleBar/EndEndTitlebarSample.html.embed"
};
constexpr char8_t endToEndCode[] = {
#include "Snippets/TitleBar/EndEndTitlebarSample.h.embed"
};

FrameworkElement endToEnd() {
#include "Snippets/TitleBar/EndEndTitlebarSample.h"

    return gallery::controlExample({
        .header = gallery::snippet(endToEndHeader),
        .example = example,
        .code = gallery::snippet(endToEndCode),
    });
}

}  // namespace

wxl::FrameworkElement gallery::titleBarPage() {
    return StackPanel {
        Margin {0, 12, 0, 0},
        gallery::prose(gallery::snippet(introText), Thickness {}), configurationExample(), dragRegions(), endToEnd()};
}
