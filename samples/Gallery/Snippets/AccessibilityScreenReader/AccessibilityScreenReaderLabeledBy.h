auto const label = TextBlock {automationAccessibilityView = AccessibilityView::Raw, styles.TextBlock.Body, u"Searching Photos:"};
auto example = StackPanel {
    spacing = 8.0,
    label,
    TextBox {width = 200, hAlign.left, automationLabeledBy = label},
    HtmlBlock {
        fontSize = 12,
        u"The TextBox above is labeled by the TextBlock and will be read as <b>Searching Photos</b>.",
    },
};
