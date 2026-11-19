// Страница XamlResources — XamlResourcesPage оригинала. Ресурсы трёх уровней, StaticResource и ThemeResource —
// у wxl это значения, resourceBrush и пути `brushes`; собственный словарь тем остаётся разметкой, и пример читает её loadXaml.

#include "Pages.h"
#include "Shell.h"
#include "Resource.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText levelsHeader{"Snippets/XamlResources/Xamlresources.html"};
gallery::LazyLoadedText levelsCode{"Snippets/XamlResources/Xamlresources.h"};
gallery::LazyLoadedText versusHeader{"Snippets/XamlResources/XamlResourcesStaticresourceVersusThemeresource.html"};
gallery::LazyLoadedText versusCode{"Snippets/XamlResources/XamlResourcesStaticresourceVersusThemeresource.h"};
gallery::LazyLoadedText themeHeader{"Snippets/XamlResources/XamlResourcesDefineNewThemeResource.html"};
gallery::LazyLoadedText themeCode{"Snippets/XamlResources/XamlResourcesDefineNewThemeResource.h"};

FrameworkElement levels() {
#include "Snippets/XamlResources/Xamlresources.h"
    return gallery::controlExample({.header = gallery::snippet(levelsHeader), .example = example, .code = gallery::snippet(levelsCode)});
}

FrameworkElement versus() {
#include "Snippets/XamlResources/XamlResourcesStaticresourceVersusThemeresource.h"
    return gallery::controlExample({.header = gallery::snippet(versusHeader), .example = example, .code = gallery::snippet(versusCode)});
}

FrameworkElement newTheme() {
#include "Snippets/XamlResources/XamlResourcesDefineNewThemeResource.h"
    return gallery::controlExample({.header = gallery::snippet(themeHeader), .example = example, .code = gallery::snippet(themeCode)});
}

}  // namespace

static FrameworkElement xamlResourcesPage() {
    return StackPanel {
        TextBlock {Margin {0, 12, 0, 4}, styles.TextBlock.Subtitle, u"Creating and using resources"},
        StackPanel {
            spacing = 12.0,
            RichTextBlock {Paragraph {Run {u"A resource is a value under a key: the key is a unique identifier, the value a color, a brush, a text. "
                                           u"In XAML it is an entry of a "},
                                      Run {fontFamily = u"Consolas", u"ResourceDictionary"}, Run {u"; here it is a value of the program."}}},
            RichTextBlock {
                Paragraph {Run {u"• App-level: resources of the framework and the application, read by key (resourceColor, resourceBrush)."}},
                Paragraph {Run {u"• Page-level: values of the function that builds the page."}},
                Paragraph {Run {u"• Control-level: a dictionary of the control itself (dsl::resources)."}},
            },
            gallery::prose(
                u8"<p><span style=\"semibold\">Tips</span></p>"
                u8"<ul>"
                u8"<li>Naming: descriptive keys should always be used for resources to make them easier to identify.</li>"
                u8"<li>Scope: values should be defined at the narrowest scope possible to improve maintainability.</li>"
                u8"</ul>",
                Thickness {}),
        },
        levels(),
        TextBlock {Margin {0, 24, 0, 4}, styles.TextBlock.Subtitle, u"Theme resources"},
        StackPanel {
            spacing = 12.0,
            gallery::prose(
                u8"<p>WinUI includes built-in theme resources for commonly used colors. See all brushes on the "
                u8"<a href=\"item:Color\">Color page</a>.</p>",
                Thickness {}),
            gallery::prose(
                u8"<ul>"
                u8"<li>A path of <span style=\"semibold\">brushes</span> is looked up again when the theme of the element changes, as {ThemeResource} is.</li>"
                u8"<li><span style=\"semibold\">ThemeDictionaries</span> give different values for light and dark themes; they are markup.</li>"
                u8"</ul>",
                Thickness {}),
        },
        versus(),
        newTheme(),
    };
}

REGISTER_GALLERY_PAGE(L"XamlResources", xamlResourcesPage);
