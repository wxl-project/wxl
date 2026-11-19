// Страница ImplicitTransition -- ImplicitTransitionPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <cmath>
#include <memory>
#include "MotionPages.h"
#include "Resource.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Input.h>
#include <wxl/Microsoft.UI.Xaml.Media.h>
#include <wxl/Microsoft.UI.Xaml.Shapes.h>
#include <wxl/Microsoft.UI.Xaml.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText opacityExampleHeader{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesOpacity.html"};
gallery::LazyLoadedText opacityExampleCode{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesOpacity.h"};

FrameworkElement opacityExample() {
#include "Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesOpacity.h"

    return gallery::controlExample({
        .header = gallery::snippet(opacityExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(opacityExampleCode),
    });
}

gallery::LazyLoadedText rotationExampleHeader{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesRotation.html"};
gallery::LazyLoadedText rotationExampleCode{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesRotation.h"};

FrameworkElement rotationExample() {
#include "Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesRotation.h"

    return gallery::controlExample({
        .header = gallery::snippet(rotationExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(rotationExampleCode),
    });
}

gallery::LazyLoadedText scaleExampleHeader{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesScale.html"};
gallery::LazyLoadedText scaleExampleCode{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesScale.h"};

FrameworkElement scaleExample() {
#include "Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesScale.h"

    return gallery::controlExample({
        .header = gallery::snippet(scaleExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(scaleExampleCode),
    });
}

gallery::LazyLoadedText translationExampleHeader{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesTranslation.html"};
gallery::LazyLoadedText translationExampleCode{"Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesTranslation.h"};

FrameworkElement translationExample() {
#include "Snippets/ImplicitTransition/ImplicitTransitionAutomaticallyAnimateChangesTranslation.h"

    return gallery::controlExample({
        .header = gallery::snippet(translationExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(translationExampleCode),
    });
}

gallery::LazyLoadedText backgroundExampleHeader{"Snippets/ImplicitTransition/ImplicitTransitionImplicitlyAnimateBackgroundChanges.html"};
gallery::LazyLoadedText backgroundExampleCode{"Snippets/ImplicitTransition/ImplicitTransitionImplicitlyAnimateBackgroundChanges.h"};

FrameworkElement backgroundExample() {
#include "Snippets/ImplicitTransition/ImplicitTransitionImplicitlyAnimateBackgroundChanges.h"

    return gallery::controlExample({
        .header = gallery::snippet(backgroundExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(backgroundExampleCode),
    });
}

gallery::LazyLoadedText themeExampleHeader{"Snippets/ImplicitTransition/ImplicitTransitionImplicitlyAnimateGridTheme.html"};
gallery::LazyLoadedText themeExampleCode{"Snippets/ImplicitTransition/ImplicitTransitionImplicitlyAnimateGridTheme.h"};

FrameworkElement themeExample() {
#include "Snippets/ImplicitTransition/ImplicitTransitionImplicitlyAnimateGridTheme.h"

    return gallery::controlExample({
        .header = gallery::snippet(themeExampleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(themeExampleCode),
    });
}

}  // namespace

static wxl::FrameworkElement implicitTransitionPage() {
    return StackPanel {opacityExample(), rotationExample(), scaleExample(), translationExample(), backgroundExample(), themeExample()};
}

REGISTER_GALLERY_PAGE(L"ImplicitTransition", implicitTransitionPage);
