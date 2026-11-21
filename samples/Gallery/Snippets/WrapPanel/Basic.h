// The options are fields of a model; the panel shows them, the sliders and
// the radio buttons write them.
struct Model {
    core::observable<int> orientation{0};
    core::observable<double> itemSpacing{8};
    core::observable<double> lineSpacing{8};
    core::observable<double> padding{12};
};
auto const model = gallery::hold<Model>();

auto item = [](char16_t const* words, double wrapWidth) {
    return Border {
        minWidth = wrapWidth,
        Padding {12, 8},
        background = brushes.Control.FillColor.Default,
        borderBrush = brushes.Control.StrokeColor.Default,
        BorderThickness {1},
        CornerRadius {4},
        TextBlock {words},
    };
};

auto layoutHost = Border {
    width = 360,
    height = 200,
    hAlign.left,
    background = brushes.Card.BackgroundFillColor.Secondary,
    borderBrush = brushes.Card.StrokeColorDefault,
    BorderThickness {1},
    CornerRadius {4},
    WrapPanel {
        orientation = BindOutput {model->orientation, [](int index) {
                                      return index == 0 ? orientation.horizontal : orientation.vertical;
                                  }},
        itemSpacing = BindOutput {model->itemSpacing},
        lineSpacing = BindOutput {model->lineSpacing},
        padding = BindOutput {model->padding, [](double amount) { return Thickness {amount}; }},
        item(u"Alpha", 72),
        item(u"Beta item", 112),
        item(u"Gamma", 88),
        item(u"Delta item", 128),
        item(u"Epsilon", 80),
        item(u"Zeta item", 104),
        item(u"Eta", 76),
        item(u"Theta item", 120),
        item(u"Iota", 92),
        item(u"Kappa item", 132),
    },
};

auto slider = [](char16_t const* headerText, char16_t const* automationText, char16_t const* id, core::observable<double>& field) {
    return Slider {
        automationId = id,
        automationName = automationText,
        header = headerText,
        minimum = 0.0,
        maximum = 24.0,
        snapsTo = SliderSnapsTo::Ticks,
        stepFrequency = 2.0,
        tickFrequency = 2.0,
        value = Bind {field},
    };
};

auto options = StackPanel {
    spacing = 12.0,
    RadioButtons {
        automationId = u"WrapPanelOrientation",
        automationName = u"WrapPanel orientation",
        header = u"Orientation",
        RadioButton {automationId = u"WrapPanelHorizontalOrientation", automationName = u"Horizontal orientation", content = u"Horizontal"},
        RadioButton {automationId = u"WrapPanelVerticalOrientation", automationName = u"Vertical orientation", content = u"Vertical"},
        selectedIndex = Bind {model->orientation},
    },
    slider(u"Item spacing", u"Item spacing", u"WrapPanelItemSpacing", model->itemSpacing),
    slider(u"Line spacing", u"Line spacing", u"WrapPanelLineSpacing", model->lineSpacing),
    slider(u"Padding", u"Uniform padding", u"WrapPanelPadding", model->padding),
};