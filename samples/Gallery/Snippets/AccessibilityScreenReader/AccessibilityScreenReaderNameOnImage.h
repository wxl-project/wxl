auto example = StackPanel {
    spacing = 12.0,
    Border {cornerRadius = CornerRadius {4},
            child = Image {height = 150, hAlign.left, automationName = u"Grapes", source = u"Assets/SampleMedia/grapes.jpg"}},
    HtmlBlock {
        fontSize = 12,
        u"The image above will be read out as <b>Grapes</b>. "
        u"To navigate in Narrator through elements that aren't focusable, use Caps + Left/Right arrow.",
    },
};
