// The source of a binding is a field of a model, an observable. The target is the property of a control: a field that a
// control only shows is bound with BindOutput (OneWay), one that a control only writes with BindInput, and one that both
// show and write with Bind (TwoWay).
struct Model {
    core::observable<core::u16_text> oneWay, twoWay;
};
auto const model = gallery::hold<Model>();

// A preset: the arguments of a box, written once and worn by four of them.
auto const box = Preset {minWidth = 380, hAlign.left};

auto description = HtmlBlock {};
description.theme(MarkupTheme{.paragraphMargin = Thickness {0}});
description.html(
    u"<p>With <b>BindOutput</b> the field is shown: what is typed into the second box stays in it.</p>"
    u"<p>With <b>Bind</b> changes in either box go through the field to the other.</p>");

auto example = StackPanel {
    spacing = 12.0,
    Grid {
        columnSpacing = 12.0,
        ColumnDefinitions<u"auto,auto,auto">,
        StackPanel {
            spacing = 8.0,
            TextBlock {FontWeight {600}, u"OneWay binding"},
            TextBox {box, placeholderText = u"Enter text here", text = BindInput {model->oneWay}},
            TextBox {box, placeholderText = u"Mirrors above text", text = BindOutput {model->oneWay}},
        },
        AppBarSeparator {column = 1},
        StackPanel {
            column = 2,
            spacing = 8.0,
            TextBlock {FontWeight {600}, u"TwoWay binding"},
            TextBox {box, placeholderText = u"Enter text here", text = Bind {model->twoWay}},
            TextBox {box, placeholderText = u"Mirrors and edits above text", text = Bind {model->twoWay}},
        },
    },
    description,
};
