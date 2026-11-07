// Страница RadialGradientBrush -- RadialGradientBrushPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText sampleHeader{"Snippets/RadialGradientBrush/RadialGradientBrushSample.html"};
gallery::LazyLoadedText sampleCode{"Snippets/RadialGradientBrush/RadialGradientBrushSample.h"};

FrameworkElement sample() {
#include "Snippets/RadialGradientBrush/RadialGradientBrushSample.h"

    return gallery::controlExample({
        .header = gallery::snippet(sampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(sampleCode),
    });
}

}  // namespace

wxl::FrameworkElement gallery::radialGradientBrushPage() {
    return StackPanel {sample()};
}
