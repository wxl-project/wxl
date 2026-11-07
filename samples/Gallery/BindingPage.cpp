// Страница Binding — BindingPage оригинала. Привязка у wxl своя: источник — поле модели (core::observable), цель — свойство
// контрола, направление выбирает написанное слово: BindOutput, BindInput или Bind. Конвертер XAML — функция рядом с привязкой.

#include "Pages.h"
#include "Shell.h"
#include "Bind.h"
#include "Box.h"
#include "IndexList.h"
#include "ItemTemplate.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText controlsHeader{"Snippets/Binding/BindingControls.html"};
gallery::LazyLoadedText controlsCode{"Snippets/Binding/BindingControls.h"};

FrameworkElement controlsExample() {
#include "Snippets/Binding/BindingControls.h"

    return gallery::controlExample({.header = gallery::snippet(controlsHeader), .example = example, .code = gallery::snippet(controlsCode)});
}

gallery::LazyLoadedText propertyFieldHeader{"Snippets/Binding/BindingPropertyCodeBehind.html"};
gallery::LazyLoadedText propertyFieldCode{"Snippets/Binding/BindingPropertyCodeBehind.h"};

FrameworkElement propertyFieldExample() {
#include "Snippets/Binding/BindingPropertyCodeBehind.h"

    return gallery::controlExample({.header = gallery::snippet(propertyFieldHeader), .example = example, .code = gallery::snippet(propertyFieldCode)});
}

gallery::LazyLoadedText functionOfFieldsHeader{"Snippets/Binding/BindingFunctionBind.html"};
gallery::LazyLoadedText functionOfFieldsCode{"Snippets/Binding/BindingFunctionBind.h"};

FrameworkElement functionOfFieldsExample() {
#include "Snippets/Binding/BindingFunctionBind.h"

    return gallery::controlExample({.header = gallery::snippet(functionOfFieldsHeader), .example = example, .code = gallery::snippet(functionOfFieldsCode)});
}

gallery::LazyLoadedText converterHeader{"Snippets/Binding/ConverterBinding.html"};
gallery::LazyLoadedText converterCode{"Snippets/Binding/ConverterBinding.h"};

FrameworkElement converterExample() {
#include "Snippets/Binding/ConverterBinding.h"

    return gallery::controlExample({.header = gallery::snippet(converterHeader), .example = example, .code = gallery::snippet(converterCode)});
}

gallery::LazyLoadedText viewModelHeader{"Snippets/Binding/BindingViewModel.html"};
gallery::LazyLoadedText viewModelCode{"Snippets/Binding/BindingViewModel.h"};

FrameworkElement viewModelExample() {
#include "Snippets/Binding/BindingViewModel.h"

    return gallery::controlExample({.header = gallery::snippet(viewModelHeader), .example = example, .code = gallery::snippet(viewModelCode)});
}

gallery::LazyLoadedText nullValueHeader{"Snippets/Binding/BindingTargetnullvalue.html"};
gallery::LazyLoadedText nullValueCode{"Snippets/Binding/BindingTargetnullvalue.h"};

FrameworkElement nullValueExample() {
#include "Snippets/Binding/BindingTargetnullvalue.h"

    return gallery::controlExample({.header = gallery::snippet(nullValueHeader), .example = example, .code = gallery::snippet(nullValueCode)});
}

gallery::LazyLoadedText masterDetailHeader{"Snippets/Binding/BindingCollectionDataTemplates.html"};
gallery::LazyLoadedText masterDetailCode{"Snippets/Binding/BindingCollectionDataTemplates.h"};

FrameworkElement masterDetailExample() {
#include "Snippets/Binding/BindingCollectionDataTemplates.h"

    return gallery::controlExample({.header = gallery::snippet(masterDetailHeader), .example = example, .code = gallery::snippet(masterDetailCode)});
}

}  // namespace

static FrameworkElement bindingPage() {
    return StackPanel {
        spacing = 12.0,
        gallery::prose(
            u8"<p><span style=\"semibold\">Key concepts</span></p>"
            u8"<ul>"
            u8"<li>Target: the property of a control to which data is bound (Text, Background, Visibility).</li>"
            u8"<li>Source: a field of a model, an observable. It is held by address and never owned by the binding: the model "
            u8"outlives the description, and the binding is a watch inside the field.</li>"
            u8"<li>Direction is the word that is written:"
            u8"<ul>"
            u8"<li>BindOutput updates the target when the source changes (OneWay).</li>"
            u8"<li>Bind updates both the target and the source (TwoWay).</li>"
            u8"<li>BindInput updates the source from the target alone.</li>"
            u8"<li>A plain value sets the target once (OneTime).</li>"
            u8"</ul>"
            u8"</li>"
            u8"</ul>",
            Thickness {}),
        RichTextBlock {Paragraph {Run {u"The Quadratic sample of wxl is built on this: three fields in, a field that follows them, answers out."}}},
        controlsExample(),
        propertyFieldExample(),
        functionOfFieldsExample(),
        converterExample(),
        viewModelExample(),
        nullValueExample(),
        masterDetailExample(),
    };
}

REGISTER_GALLERY_PAGE(L"Binding", bindingPage);
