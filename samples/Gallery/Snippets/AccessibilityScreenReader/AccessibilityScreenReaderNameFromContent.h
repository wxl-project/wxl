auto example = StackPanel {
    spacing = 12.0,
    Button {content = u"Download survey"},
    HtmlBlock {
        fontSize = 12,
        u"Screen readers will read this button as <b>Download survey</b>. "
        u"The name is automatically derived from the Button's Content.",
    },
};
