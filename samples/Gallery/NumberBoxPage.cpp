// Страница NumberBox — NumberBoxPage оригинала: три примера.

#include "Pages.h"
#include "Shell.h"

#include <limits>

#include <wxl/Windows.Globalization.NumberFormatting.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText expressionsHeader{"Snippets/NumberBox/EvaluatesExpressions.html"};
gallery::LazyLoadedText expressionsCode{"Snippets/NumberBox/EvaluatesExpressions.h"};

FrameworkElement expressions() {
    return gallery::controlExample({
        .header = gallery::snippet(expressionsHeader),
        .example =
#include "Snippets/NumberBox/EvaluatesExpressions.h"
        ,
        .code = gallery::snippet(expressionsCode),
    });
}

gallery::LazyLoadedText spinHeader{"Snippets/NumberBox/SpinButton.html"};
gallery::LazyLoadedText spinCode{"Snippets/NumberBox/SpinButton.h"};

FrameworkElement spinButton() {
#include "Snippets/NumberBox/SpinButton.h"

    return gallery::controlExample({
        .header = gallery::snippet(spinHeader),
        .example = number,
        .options = {placement},
        .code = gallery::snippet(spinCode),
    });
}

gallery::LazyLoadedText formattedHeader{"Snippets/NumberBox/FormattedRoundsNearest.html"};
gallery::LazyLoadedText formattedCode{"Snippets/NumberBox/FormattedRoundsNearest.h"};

FrameworkElement formatted() {
#include "Snippets/NumberBox/FormattedRoundsNearest.h"

    return gallery::controlExample({
        .header = gallery::snippet(formattedHeader),
        .example = amount,
        .code = gallery::snippet(formattedCode),
    });
}

}  // namespace

static wxl::FrameworkElement numberBoxPage() {
    return StackPanel {expressions(), spinButton(), formatted()};
}

REGISTER_GALLERY_PAGE(L"NumberBox", numberBoxPage);
