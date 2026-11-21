// Страница Color — ColorPage оригинала: пример ссылки на кисть, SelectorBar из шести разделов и рамка,
// в которую раздел входит с перелистыванием вбок.

#include "Pages.h"

#include "ColorTile.h"
#include "NavigationPages.h"
#include "Shell.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.Animation.h>

using namespace wxl;

namespace {

gallery::LazyLoadedText referenceHeader{"Snippets/Color/ColorBrushReference.html"};
gallery::LazyLoadedText referenceCode{"Snippets/Color/ColorBrushReference.h"};

FrameworkElement section(int index) {
    switch (index) {
        case 1: return gallery::fillSection();
        case 2: return gallery::strokeSection();
        case 3: return gallery::backgroundSection();
        case 4: return gallery::signalSection();
        case 5: return gallery::highContrastSection();
        default: return gallery::textSection();
    }
}

struct Model {
    int previous = 0;
    Frame frame {dsl::isNavigationStackEnabled = false, dsl::vAlign.stretch};
};

}  // namespace

static FrameworkElement colorPage() {
    using namespace wxl::dsl;
#include "Snippets/Color/ColorBrushReference.h"

    auto const model = gallery::hold<Model>();
    auto const bar = SelectorBar {
        Margin {-12, 0, 0, 0},
        automationName = u"PageSelector",
        items[SelectorBarItem {text = u"Text", isSelected = true}, SelectorBarItem {text = u"Fill"}, SelectorBarItem {text = u"Stroke"},
              SelectorBarItem {text = u"Background"}, SelectorBarItem {text = u"Signal"}, SelectorBarItem {text = u"High Contrast"}],
        onSelectionChanged += [model](SelectorBar const& sender, SelectorBarSelectionChangedEventArgs&) {
            auto const selected = sender.selectedItem();
            auto const barItems = sender.items();
            int current = 0;
            for (uint32_t i = 0; i < barItems.size(); ++i) {
                if (barItems[i].text() == selected.text()) {
                    current = static_cast<int>(i);
                }
            }
            auto const slide = current - model->previous > 0 ? SlideNavigationTransitionEffect::FromRight : SlideNavigationTransitionEffect::FromLeft;
            navigatePage(model->frame, SlideNavigationTransitionInfo {effect = slide}).content(section(current));
            model->previous = current;
        },
    };
    navigatePage(model->frame).content(section(0));

    return StackPanel {
        gallery::controlExample({
            .header = gallery::snippet(referenceHeader),
            .example = example,
            .code = gallery::snippet(referenceCode),
        }),
        bar,
        model->frame,
    };
}

REGISTER_GALLERY_PAGE(L"Color", colorPage);
