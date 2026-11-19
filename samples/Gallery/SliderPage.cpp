// Страница Slider — SliderPage оригинала: четыре примера.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/Slider/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/Slider/Simple.h"};

FrameworkElement simple() {
#include "Snippets/Slider/Simple.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = StackPanel {orientation.horizontal, slider},
        .output = {output},
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText rangeHeader{"Snippets/Slider/Range.html"};
gallery::LazyLoadedText rangeCode{"Snippets/Slider/Range.h"};

FrameworkElement range() {
#include "Snippets/Slider/Range.h"

    return gallery::controlExample({
        .header = gallery::snippet(rangeHeader),
        .example = StackPanel {vAlign.center, orientation.horizontal, slider},
        .output = {output},
        .options = {options},
        .code = gallery::snippet(rangeCode),
    });
}

gallery::LazyLoadedText ticksHeader{"Snippets/Slider/Ticks.html"};
gallery::LazyLoadedText ticksCode{"Snippets/Slider/Ticks.h"};

FrameworkElement ticks() {
#include "Snippets/Slider/Ticks.h"

    return gallery::controlExample({
        .header = gallery::snippet(ticksHeader),
        .example = StackPanel {orientation.horizontal, slider},
        .output = {output},
        .options = {StackPanel {snaps}},
        .code = gallery::snippet(ticksCode),
    });
}

gallery::LazyLoadedText verticalHeader{"Snippets/Slider/Vertical.html"};
gallery::LazyLoadedText verticalCode{"Snippets/Slider/Vertical.h"};

FrameworkElement vertical() {
#include "Snippets/Slider/Vertical.h"

    return gallery::controlExample({
        .header = gallery::snippet(verticalHeader),
        .example = StackPanel {orientation.horizontal, slider},
        .output = {output},
        .code = gallery::snippet(verticalCode),
    });
}

}  // namespace

static wxl::FrameworkElement sliderPage() {
    return StackPanel {simple(), range(), ticks(), vertical()};
}

REGISTER_GALLERY_PAGE(L"Slider", sliderPage);
