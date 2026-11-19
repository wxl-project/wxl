// Страница Templates — TemplatesPage оригинала и то, что у wxl на этом месте: ControlTemplate и ItemsPanelTemplate
// остаются разметкой (loadXaml), DataTemplate — функция от элемента, а описание строится из Preset и Template,
// вложенных друг в друга и в настоящие описания.

#include "Pages.h"
#include "Resource.h"
#include "ItemTemplate.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>
#include <wxl/Microsoft.UI.Xaml.Media.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText controlHeader{"Snippets/Templates/TemplatesCustomizeLookTextboxControltemplate.html"};
gallery::LazyLoadedText controlCode{"Snippets/Templates/TemplatesCustomizeLookTextboxControltemplate.h"};
gallery::LazyLoadedText dataHeader{"Snippets/Templates/TemplatesCustomizeComboboxItemtemplateDatatemplate.html"};
gallery::LazyLoadedText dataCode{"Snippets/Templates/TemplatesCustomizeComboboxItemtemplateDatatemplate.h"};
gallery::LazyLoadedText panelHeader{"Snippets/Templates/TemplatesCustomizeItemscontrolItemspaneltemplate.html"};
gallery::LazyLoadedText panelCode{"Snippets/Templates/TemplatesCustomizeItemscontrolItemspaneltemplate.h"};
gallery::LazyLoadedText presetsHeader{"Snippets/Templates/TemplatesPresetsNested.html"};
gallery::LazyLoadedText presetsCode{"Snippets/Templates/TemplatesPresetsNested.h"};
gallery::LazyLoadedText templatesHeader{"Snippets/Templates/TemplatesBuiltWhereApplied.html"};
gallery::LazyLoadedText templatesCode{"Snippets/Templates/TemplatesBuiltWhereApplied.h"};

FrameworkElement controlTemplate() {
#include "Snippets/Templates/TemplatesCustomizeLookTextboxControltemplate.h"
    return gallery::controlExample({.header = gallery::snippet(controlHeader), .example = example, .code = gallery::snippet(controlCode)});
}

FrameworkElement dataTemplate() {
#include "Snippets/Templates/TemplatesCustomizeComboboxItemtemplateDatatemplate.h"
    return gallery::controlExample({.header = gallery::snippet(dataHeader), .example = example, .code = gallery::snippet(dataCode)});
}

FrameworkElement itemsPanelTemplate() {
#include "Snippets/Templates/TemplatesCustomizeItemscontrolItemspaneltemplate.h"
    return gallery::controlExample({.header = gallery::snippet(panelHeader), .example = example, .code = gallery::snippet(panelCode)});
}

FrameworkElement nestedPresets() {
#include "Snippets/Templates/TemplatesPresetsNested.h"
    return gallery::controlExample({.header = gallery::snippet(presetsHeader), .example = example, .code = gallery::snippet(presetsCode)});
}

FrameworkElement builtTemplates() {
#include "Snippets/Templates/TemplatesBuiltWhereApplied.h"
    return gallery::controlExample({.header = gallery::snippet(templatesHeader), .example = example, .code = gallery::snippet(templatesCode)});
}

}  // namespace

static FrameworkElement templatesPage() {
    return StackPanel {
        spacing = 12.0,
        Margin {0, 12, 0, 0},
        gallery::prose(
            u8"<p><span style=\"semibold\">Templates of XAML</span></p>"
            u8"<p>There are 3 types of templates in XAML:</p>"
            u8"<ul>"
            u8"<li>ControlTemplate: customizes the structure of a control. It names the parts of the control, so it is markup.</li>"
            u8"<li>DataTemplate: changes how individual items are displayed in a control like a ComboBox or ListView. "
            u8"Here it is a function from the item to an element.</li>"
            u8"<li>ItemsPanelTemplate: defines how a collection of items is laid out. Markup as well.</li>"
            u8"</ul>",
            Thickness {}),
        controlTemplate(),
        dataTemplate(),
        itemsPanelTemplate(),
        TextBlock {Margin {0, 24, 0, 0}, styles.TextBlock.Subtitle, u"Presets and templates of wxl"},
        RichTextBlock {
            Paragraph {Run {u"What a style and a template of XAML are for -- giving a look a name and using it again -- wxl does with two "
                            u"types, and they differ in what they keep."}},
            Paragraph {Bold {Run {u"Preset"}}, Run {u" keeps its arguments: the properties, unnamed values, handlers and children written in "
                                                  u"its braces, as they were written. It is worn by whatever has those properties, either "
                                                  u"written unnamed inside braces or applied to an object that exists (Apply)."}},
            Paragraph {Bold {Run {u"Template<T>"}}, Run {u" keeps its arguments too, but builds an object of T where it is applied, and builds a "
                                                       u"new one every time. A live object -- a child, a brush -- written in a preset would be made "
                                                       u"once and handed to the framework twice; a template is the way to write it."}},
            Paragraph {Run {u"Both stand wherever an argument stands, so they nest: presets in presets, presets in templates, templates in "
                            u"templates, templates in presets, and each in a real description."}},
        },
        nestedPresets(),
        builtTemplates(),
    };
}

REGISTER_GALLERY_PAGE(L"Templates", templatesPage);
