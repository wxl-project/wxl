auto example = StackPanel {
    spacing = 12.0,
    // Headers are promoted to name
    TextBox {width = 200, hAlign.left, header = u"Name"},
    // As are placeholders
    TextBox {width = 200, hAlign.left, placeholderText = u"Nickname"},
    // If both are provided, headers are name and placeholders are moved to description
    TextBox {minWidth = 200, hAlign.left, header = u"Email", placeholderText = u"test@example.com"},
    HtmlBlock {
        fontSize = 12,
        u"<p>Screen readers will read these TextBoxes as <b>Name</b>, <b>Nickname</b> and <b>Email</b>. "
        u"The names are automatically derived from their headers or placeholders.</p>"
        u"<p>When both Header and PlaceholderText are present, Header is used as name and PlaceholderText is used as description.</p>",
    },
};
