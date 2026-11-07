// Страница AnimatedIcon -- AnimatedIconPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.AnimatedVisuals.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText buttonHeader{"Snippets/AnimatedIcon/AddingAnimatediconButton.html"};
gallery::LazyLoadedText buttonCode{"Snippets/AnimatedIcon/AddingAnimatediconButton.h"};

FrameworkElement button() {
#include "Snippets/AnimatedIcon/AddingAnimatediconButton.h"

    return gallery::controlExample({
        .header = gallery::snippet(buttonHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(buttonCode),
    });
}

gallery::LazyLoadedText navigationViewHeader{"Snippets/AnimatedIcon/AddingAnimatediconNavigationview.html"};
gallery::LazyLoadedText navigationViewCode{"Snippets/AnimatedIcon/AddingAnimatediconNavigationview.h"};

FrameworkElement navigationView() {
#include "Snippets/AnimatedIcon/AddingAnimatediconNavigationview.h"

    return gallery::controlExample({
        .header = gallery::snippet(navigationViewHeader),
        .example = example,
        .code = gallery::snippet(navigationViewCode),
    });
}

}  // namespace

static wxl::FrameworkElement animatedIconPage() {
    return StackPanel {button(), navigationView()};
}

REGISTER_GALLERY_PAGE(L"AnimatedIcon", animatedIconPage);
