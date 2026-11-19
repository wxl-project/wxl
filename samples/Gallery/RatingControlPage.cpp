// Страница RatingControl — RatingControlPage оригинала: два примера.

#include "Pages.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/RatingControl/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/RatingControl/Simple.h"};

FrameworkElement simple() {
#include "Snippets/RatingControl/Simple.h"

    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example = StackPanel {vAlign.top, rating},
        .output = {output},
        .options = {options},
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText placeholderHeader{"Snippets/RatingControl/Placeholder.html"};
gallery::LazyLoadedText placeholderCode{"Snippets/RatingControl/Placeholder.h"};

FrameworkElement placeholder() {
#include "Snippets/RatingControl/Placeholder.h"

    return gallery::controlExample({
        .header = gallery::snippet(placeholderHeader),
        .example = rating,
        .options = {StackPanel {width = 220, slider}},
        .code = gallery::snippet(placeholderCode),
    });
}

}  // namespace

static wxl::FrameworkElement ratingControlPage() {
    return StackPanel {simple(), placeholder()};
}

REGISTER_GALLERY_PAGE(L"RatingControl", ratingControlPage);
