// Страница Bound collection — своя у wxl, рядом с Binding: списочный контрол привязан к списку модели
// (core::observable_list) одним значением, itemsSource = BindOutput {list, build}. Список растёт кнопкой, строка элемента
// меняется полем самого элемента, фильтр пересобирает список целиком.

#include "Pages.h"
#include "Shell.h"
#include "Bind.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText growingHeader{"Snippets/BoundCollection/BoundCollectionGrowing.html"};
gallery::LazyLoadedText growingCode{"Snippets/BoundCollection/BoundCollectionGrowing.h"};

FrameworkElement growingExample() {
#include "Snippets/BoundCollection/BoundCollectionGrowing.h"

    return gallery::controlExample({.header = gallery::snippet(growingHeader), .example = example, .code = gallery::snippet(growingCode)});
}

gallery::LazyLoadedText itemFieldHeader{"Snippets/BoundCollection/BoundCollectionItemField.html"};
gallery::LazyLoadedText itemFieldCode{"Snippets/BoundCollection/BoundCollectionItemField.h"};

FrameworkElement itemFieldExample() {
#include "Snippets/BoundCollection/BoundCollectionItemField.h"

    return gallery::controlExample({.header = gallery::snippet(itemFieldHeader), .example = example, .code = gallery::snippet(itemFieldCode)});
}

gallery::LazyLoadedText filterHeader{"Snippets/BoundCollection/BoundCollectionFilter.html"};
gallery::LazyLoadedText filterCode{"Snippets/BoundCollection/BoundCollectionFilter.h"};

FrameworkElement filterExample() {
#include "Snippets/BoundCollection/BoundCollectionFilter.h"

    return gallery::controlExample({.header = gallery::snippet(filterHeader), .example = example, .code = gallery::snippet(filterCode)});
}

}  // namespace

static FrameworkElement boundCollectionPage() {
    return StackPanel {
        spacing = 12.0,
        gallery::prose(
            u8"<p><span style=\"semibold\">Key concepts</span></p>"
            u8"<ul>"
            u8"<li>Source: a list of a model, core::observable_list. Every change says what happened and where: "
            u8"items inserted, erased, replaced, or the whole list anew.</li>"
            u8"<li>Target: the items of a ListView, a GridView, an ItemsView or an ItemsRepeater, with the function that "
            u8"builds the element of an item: itemsSource = BindOutput {list, build}. The type of the items is checked "
            u8"against the function where the binding is written.</li>"
            u8"<li>The control builds the elements in view only, and each change of the list reaches it as that change.</li>"
            u8"<li>An element may bind to the fields of its item; those bindings go when the control gives the element back.</li>"
            u8"<li>boundItem(list, item) is the item of the model behind what the control hands out (clickedItem, "
            u8"selectedItem).</li>"
            u8"</ul>",
            Thickness {}),
        growingExample(),
        itemFieldExample(),
        filterExample(),
    };
}

REGISTER_GALLERY_PAGE(L"BoundCollection", boundCollectionPage);
