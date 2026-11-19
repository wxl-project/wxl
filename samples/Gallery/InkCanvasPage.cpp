// Страница InkCanvas -- InkCanvasPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <algorithm>
#include <initializer_list>
#include <string>
#include "RepeaterData.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.h>
#include <wxl/Windows.Storage.Streams.h>
#include <wxl/Windows.UI.Core.Enums.h>
#include <wxl/Windows.UI.Input.Inking.h>

import wxl.core;


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText drawingHeader{"Snippets/InkCanvas/InkCanvasDrawing.html"};
gallery::LazyLoadedText drawingCode{"Snippets/InkCanvas/InkCanvasDrawing.h"};

FrameworkElement drawing() {
#include "Snippets/InkCanvas/InkCanvasDrawing.h"

    return gallery::controlExample({
        .header = gallery::snippet(drawingHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(drawingCode),
    });
}

gallery::LazyLoadedText toolbarHeader{"Snippets/InkCanvas/InkCanvasToolbar.html"};
gallery::LazyLoadedText toolbarCode{"Snippets/InkCanvas/InkCanvasToolbar.h"};

FrameworkElement toolbar() {
#include "Snippets/InkCanvas/InkCanvasToolbar.h"

    return gallery::controlExample({
        .header = gallery::snippet(toolbarHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(toolbarCode),
    });
}

gallery::LazyLoadedText strokesHeader{"Snippets/InkCanvas/InkCanvasStrokes.html"};
gallery::LazyLoadedText strokesCode{"Snippets/InkCanvas/InkCanvasStrokes.h"};

FrameworkElement strokes() {
#include "Snippets/InkCanvas/InkCanvasStrokes.h"

    return gallery::controlExample({
        .header = gallery::snippet(strokesHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(strokesCode),
    });
}

}  // namespace

static wxl::FrameworkElement inkCanvasPage() {
    return StackPanel {drawing(), toolbar(), strokes()};
}

REGISTER_GALLERY_PAGE(L"InkCanvas", inkCanvasPage);
