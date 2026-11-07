struct Model {
    core::observable<int> mode {1};
};
auto const model = gallery::hold<Model>();

// The ContactListViewTemplate of the original -- an ellipse, a name over a company -- as a function from the
// item (the position of a contact in the application's vector) to the element that shows it.
auto const* people = &gallery::contacts();
auto const contactOf = [people](Object const& item) {
    auto const& contact = (*people)[static_cast<size_t>(unboxValue<int64_t>(item))];
    return Grid {
        RowDefinitions<u"*,*">,
        ColumnDefinitions<u"auto,*">,
        Ellipse {rowSpan = 2, width = 32, height = 32, Margin {6}, horizontalAlignment = HorizontalAlignment::Center,
                 verticalAlignment = VerticalAlignment::Center, fill = brushes.Control.Strong.FillColorDefault},
        TextBlock {column = 1, Margin {12, 6, 0, 0}, styles.TextBlock.Base, contact.name()},
        TextBlock {row = 1, column = 1, Margin {12, 0, 0, 6}, styles.TextBlock.Body, contact.company},
    };
};
auto const list = ListView {
    width = 400,
    height = 400,
    horizontalAlignment = HorizontalAlignment::Left,
    borderBrush = brushes.Control.Strong.StrokeColorDefault,
    borderThickness = 1,
    selectionMode = BindOutput {model->mode, [](int index) { return static_cast<ListViewSelectionMode>(index); }},
    itemTemplate = contactOf,
    itemsSource = IndexList(static_cast<int64_t>(people->size())),
};

auto description = HtmlBlock {
    Margin {0, 0, 0, 15},
    textWrapping = TextWrapping::Wrap,
};
description.theme(MarkupTheme{.paragraphMargin = Thickness {0}});
description.html(
    u"<p>You can enable four different selection modes on the right.</p>"
    u"<p><b>None</b> disables selection all together.</p>"
    u"<p><b>Single</b> allows for only one item to be selected in the list.</p>"
    u"<p><b>Multiple</b> causes checkboxes to appear next to items, so that multiple items can be chosen from the list.</p>"
    u"<p><b>Extended</b> allows the user to select multiple items by using Ctrl+Click to select the individual items they want, "
    u"or Shift+Click to select a range of contiguous items.</p>");

auto example = StackPanel {
    description,
    list,
};

auto options = ComboBox {
    Margin {0, 12, 0, 0},
    header = u"SelectionMode",
    ComboBoxItem {content = u"None"},
    ComboBoxItem {content = u"Single"},
    ComboBoxItem {content = u"Multiple"},
    ComboBoxItem {content = u"Extended"},
    selectedIndex = Bind {model->mode},
};