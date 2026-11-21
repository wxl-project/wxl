// Страница CompositionWindow -- CompositionWindowPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "Application.h"
#include "CompositionWindow.h"
#include <d2d1_1.h>

using namespace wxl;

namespace {

gallery::LazyLoadedText creatingHeader{"Snippets/CompositionWindow/CreatingWindow.html"};
gallery::LazyLoadedText creatingCode{"Snippets/CompositionWindow/CreatingWindow.h"};

FrameworkElement creating() {
    using namespace wxl::dsl;
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
    using namespace wxl::dsl;
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
    using namespace wxl::dsl;
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
    using namespace wxl::dsl;
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
    using namespace wxl::dsl;
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
