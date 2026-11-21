auto behind = [] {
    return Grid {
        Rectangle {width = 100, height = 200, hAlign.left, vAlign.top, fill = rgb(0, 255, 255)},
        Ellipse {width = 152, height = 152, hAlign.center, vAlign.center, fill = rgb(255, 0, 255)},
        Rectangle {width = 80, height = 100, hAlign.right, vAlign.bottom, fill = rgb(255, 255, 0)},
    };
};

// A swatch and its name, as a list row.
auto swatch = [](Color c, char16_t const* colorName) {
    return ComboBoxItem {
        content = StackPanel {
            orientation.horizontal,
            Rectangle {width = 20, height = 20, fill = c},
            TextBlock {Margin {4, 0, 0, 0}, colorName},
        },
    };
};

auto acrylic = AcrylicBrush {tintColor = colors.black, tintOpacity = 0.8, fallbackColor = colors.green};

// The width follows the room the page has: 500 from a room of 500, 652 from 800, its own below that.
struct Room {
    core::observable<int> step;
};
auto const room = gallery::hold<Room>();
room->step.follow(gallery::pageSize().width, [](double pageWidth) { return pageWidth >= 800 ? 2 : pageWidth >= 500 ? 1 : 0; });

auto example = Grid {
    minWidth = 320,
    width = BindOutput {room->step, [](int step) { return step == 2 ? 652.0 : step == 1 ? 500.0 : std::numeric_limits<double>::quiet_NaN(); }},
    minHeight = BindOutput {room->step, [](int step) { return step == 0 ? 200.0 : 252.0; }},
    ColumnDefinitions<u"*,252">,
    behind(),
    Rectangle {Margin {12}, fill = acrylic},
};

auto options = StackPanel {
    TextBlock {Margin {0, 0, 0, 12}, u"Tint Opacity :"},
    Slider {
        width = 200,
        hAlign.left,
        automationName = u"tint opacity",
        isFocusEngagementEnabled = false,
        minimum = 0.0,
        maximum = 1.0,
        smallChange = 0.001,
        stepFrequency = 0.001,
        value = 0.8,
        onValueChanged += [acrylic](Slider const& self) { acrylic.tintOpacity(self.value()); },
    },
    TextBlock {Margin {0, 12}, u"Tint Color :"},
    ComboBox {
        automationName = u"tint color",
        swatch(colors.black, u"#FF000000"),
        swatch(colors.red, u"#FFFF0000"),
        swatch(colors.blue, u"#FF0000FF"),
        selectedIndex = 0,
        onSelectionChanged += [acrylic](ComboBox const& self) {
            static constexpr Color tints[] = {colors.black, colors.red, colors.blue};
            acrylic.tintColor(tints[self.selectedIndex()]);
        },
    },
    TextBlock {Margin {0, 12, 0, 12}, u"Fallback Color :"},
    ComboBox {
        automationName = u"fallback color",
        swatch(colors.green, u"#FF008000"),
        swatch(rgb(255, 255, 0), u"#FFFFFF00"),
        selectedIndex = 0,
        onSelectionChanged += [acrylic](ComboBox const& self) {
            static constexpr Color fallbacks[] = {colors.green, rgb(255, 255, 0)};
            acrylic.fallbackColor(fallbacks[self.selectedIndex()]);
        },
    },
};