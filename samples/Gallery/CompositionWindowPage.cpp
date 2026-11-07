// Страница CompositionWindow -- CompositionWindowPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "Application.h"
#include "CompositionWindow.h"
#include <d2d1_1.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText creatingHeader{"Snippets/CompositionWindow/CreatingWindow.html"};
gallery::LazyLoadedText creatingCode{"Snippets/CompositionWindow/CreatingWindow.h"};

FrameworkElement creating() {
#include "Snippets/CompositionWindow/CreatingWindow.h"

    return gallery::controlExample({
        .header = gallery::snippet(creatingHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(creatingCode),
    });
}

gallery::LazyLoadedText sceneBackgroundHeader{"Snippets/CompositionWindow/SceneBackground.html"};
gallery::LazyLoadedText sceneBackgroundCode{"Snippets/CompositionWindow/SceneBackground.h"};

FrameworkElement sceneBackground() {
#include "Snippets/CompositionWindow/SceneBackground.h"

    return gallery::controlExample({
        .header = gallery::snippet(sceneBackgroundHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(sceneBackgroundCode),
    });
}

gallery::LazyLoadedText visualsHeader{"Snippets/CompositionWindow/SceneVisuals.html"};
gallery::LazyLoadedText visualsCode{"Snippets/CompositionWindow/SceneVisuals.h"};

FrameworkElement visuals() {
#include "Snippets/CompositionWindow/SceneVisuals.h"

    return gallery::controlExample({
        .header = gallery::snippet(visualsHeader),
        .example = example,
        .code = gallery::snippet(visualsCode),
    });
}

gallery::LazyLoadedText sizeAndModeHeader{"Snippets/CompositionWindow/SizeZoomAndMode.html"};
gallery::LazyLoadedText sizeAndModeCode{"Snippets/CompositionWindow/SizeZoomAndMode.h"};

FrameworkElement sizeAndMode() {
#include "Snippets/CompositionWindow/SizeZoomAndMode.h"

    return gallery::controlExample({
        .header = gallery::snippet(sizeAndModeHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(sizeAndModeCode),
    });
}

gallery::LazyLoadedText inputHeader{"Snippets/CompositionWindow/InputDropsAndPlace.html"};
gallery::LazyLoadedText inputCode{"Snippets/CompositionWindow/InputDropsAndPlace.h"};

FrameworkElement input() {
#include "Snippets/CompositionWindow/InputDropsAndPlace.h"

    return gallery::controlExample({
        .header = gallery::snippet(inputHeader),
        .example = example,
        .code = gallery::snippet(inputCode),
    });
}

}  // namespace

static wxl::FrameworkElement compositionWindowPage() {
    return StackPanel {creating(), sceneBackground(), visuals(), sizeAndMode(), input()};
}

REGISTER_GALLERY_PAGE(L"CompositionWindow", compositionWindowPage);
