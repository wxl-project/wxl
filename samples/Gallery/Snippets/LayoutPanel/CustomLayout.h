struct Model {
    core::observable<int> layout{0};
    core::observable<double> spacing{8};
};
auto const model = gallery::hold<Model>();

// FeaturedTileLayout of the original: the first child takes the whole width,
// the rest stand in two columns, a pair to a row as tall as the taller of the two.
auto rowHeight = [](Collection<UIElement> const& panelChildren, uint32_t first) {
    float tallest = panelChildren[first].desiredSize().height;
    return first + 1 < panelChildren.size() ? std::max(tallest, panelChildren[first + 1].desiredSize().height) : tallest;
};

auto featured = CustomLayout {
    [model, rowHeight](Collection<UIElement> const& panelChildren, Size available) {
        float const gap = static_cast<float>(model->spacing.get());
        float const fullWidth = std::isinf(available.width) ? 520.0f : available.width;
        float const columnWidth = std::max(0.0f, (fullWidth - gap) / 2);

        for (uint32_t index = 0; index < panelChildren.size(); ++index) {
            panelChildren[index].measure({index == 0 ? fullWidth : columnWidth, std::numeric_limits<float>::infinity()});
        }

        float total = panelChildren.empty() ? 0.0f : panelChildren[0].desiredSize().height;
        for (uint32_t index = 1; index < panelChildren.size(); index += 2) {
            total += gap + rowHeight(panelChildren, index);
        }
        return Size {fullWidth, total};
    },
    [model, rowHeight](Collection<UIElement> const& panelChildren, Size final) {
        if (panelChildren.empty()) {
            return final;
        }
        float const gap = static_cast<float>(model->spacing.get());
        float const columnWidth = std::max(0.0f, (final.width - gap) / 2);
        float cursorY = panelChildren[0].desiredSize().height;
        panelChildren[0].arrange({{0, 0}, {final.width, cursorY}});

        for (uint32_t index = 1; index < panelChildren.size(); index += 2) {
            cursorY += gap;
            float const tallest = rowHeight(panelChildren, index);
            panelChildren[index].arrange({{0, cursorY}, {columnWidth, tallest}});
            if (index + 1 < panelChildren.size()) {
                panelChildren[index + 1].arrange({{columnWidth + gap, cursorY}, {columnWidth, tallest}});
            }
            cursorY += tallest;
        }
        return Size {final.width, cursorY};
    },
};

auto stack = StackLayout {orientation.vertical, spacing = BindOutput {model->spacing}};

auto tile = [](double extent, char16_t const* words) {
    return Border {
        height = extent,
        Padding {12},
        background = brushes.Control.FillColor.Default,
        CornerRadius {4},
        TextBlock {words, vAlign.center},
    };
};

auto panel = LayoutPanel {
    maxWidth = 560,
    hAlign.stretch,
    borderBrush = brushes.Card.StrokeColorDefault,
    BorderThickness {1},
    CornerRadius {4},
    Padding {12},
    layout = BindOutput {model->layout, [featured, stack](int chosen) -> Layout {
                             if (chosen == 0) {
                                 return featured;
                             }
                             return stack;
                         }},
    Border {
        height = 96,
        Padding {16},
        background = brushes.Accent.FillColor.Default,
        CornerRadius {4},
        StackPanel {
            vAlign.center,
            TextBlock {u"Featured item", foreground = brushes.Text.OnAccent.FillColor.Primary, fontWeight = FontWeight {600}},
            TextBlock {u"The first child spans the available width.", foreground = brushes.Text.OnAccent.FillColor.Secondary},
        },
    },
    tile(72, u"Second"),
    tile(92, u"Third (taller)"),
    tile(80, u"Fourth"),
    tile(64, u"Fifth"),
    tile(88, u"Sixth (taller)"),
    tile(68, u"Seventh"),
    tile(76, u"Eighth"),
    tile(96, u"Ninth (taller)"),
};

auto options = StackPanel {
    width = 220,
    spacing = 12.0,
    RadioButtons {
        automationId = u"LayoutPanelLayoutSelector",
        automationName = u"LayoutPanel layout",
        header = u"Layout",
        RadioButton {automationId = u"LayoutPanelFeaturedLayout", automationName = u"Featured tile layout", content = u"Featured tile"},
        RadioButton {automationId = u"LayoutPanelStackLayout", automationName = u"Vertical StackLayout", content = u"Vertical stack"},
        selectedIndex = Bind {model->layout},
    },
    Slider {
        automationId = u"LayoutPanelSpacing",
        automationName = u"Layout spacing",
        header = u"Spacing",
        maximum = 24.0,
        snapsTo = SliderSnapsTo::Ticks,
        stepFrequency = 2.0,
        tickFrequency = 2.0,
        value = Bind {model->spacing},
        onValueChanged += [featured](Object const&, RangeBaseValueChangedEventArgs&) { featured.invalidate(); },
    },
    TextBlock {
        u"LayoutPanel also applies its BorderBrush, BorderThickness, CornerRadius, and Padding around the layout.",
        textWrapping = TextWrapping::Wrap,
    },
};