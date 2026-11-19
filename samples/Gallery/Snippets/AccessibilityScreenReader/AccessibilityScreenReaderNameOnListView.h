auto example = StackPanel {
    spacing = 12.0,
    ListView {
        width = 300,
        hAlign.left,
        automationName = u"Contacts",
        items[ListViewItem {content = u"Nathan Quinn"}, ListViewItem {content = u"Jessica Lamber"}, ListViewItem {content = u"Carl Bond"},
              ListViewItem {content = u"Jessica Russel"}],
    },
    HtmlBlock {
        fontSize = 12,
        u"Screen readers will read this ListView as <b>Contacts</b>, "
        u"while each item will be read using its respective text content.",
    },
};
