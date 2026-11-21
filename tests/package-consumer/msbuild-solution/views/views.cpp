#include "views.h"

namespace views {

using namespace wxl;

// The handlers refer to the model without owning it: the program keeps it as long as the window.

StackPanel header(model::Model& model) {
    using namespace wxl::dsl;
    return StackPanel {
        spacing = 4,
        TextBlock {styles.TextBlock.Subtitle, text = BindOutput {model.title}},
        TextBlock {text = BindOutput {model.items.count(), [](std::uint32_t count) { return core::format(u"{} items", count); }}},
    };
}

ListView itemList(model::Model& model) {
    using namespace wxl::dsl;
    return ListView {
        height = 240,
        selectionMode = ListViewSelectionMode::None,
        itemsSource = BindOutput {model.items, [](core::u16_text const& item) { return TextBlock {text = item}; }},
    };
}

StackPanel editor(model::Model& model) {
    using namespace wxl::dsl;
    return StackPanel {
        orientation.horizontal,
        spacing = 8,
        TextBox {width = 240, placeholderText = u"New item", text = Bind {model.draft}},
        Button {content = u"Add", onClick += [&model] { model.add(); }},
        Button {content = u"Remove first", onClick += [&model] { model.removeFirst(); }},
    };
}

}
