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
        RichTextBlock {
            Paragraph {fontWeight = FontWeight {600}, Run {u"Key concepts"}},
            Paragraph {Run {u"\u2022 Target: the property of a control to which data is bound (Text, Background, Visibility)."}},
            Paragraph {Run {u"\u2022 Source: a field of a model, an observable. It is held by address and never owned by the binding: the model "
                            u"outlives the description, and the binding is a watch inside the field."}},
            Paragraph {Run {u"\u2022 Direction is the word that is written:"}},
            Paragraph {Run {u"    \u25E6 BindOutput updates the target when the source changes (OneWay)."}},
            Paragraph {Run {u"    \u25E6 Bind updates both the target and the source (TwoWay)."}},
            Paragraph {Run {u"    \u25E6 BindInput updates the source from the target alone."}},
            Paragraph {Run {u"    \u25E6 A plain value sets the target once (OneTime)."}},
        },
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
