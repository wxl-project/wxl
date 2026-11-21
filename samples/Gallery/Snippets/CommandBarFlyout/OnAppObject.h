auto output = TextBlock {};

auto makeCommand = [output](Symbol which, char16_t const* labelText) {
    return AppBarButton {
        icon = SymbolIcon {symbol = which},
        label = labelText,
        toolTip = labelText,
        onClick += [output, labelText](AppBarButton const&) { output.text(std::u16string {u"You clicked: "} + labelText); },
    };
};

auto plain = [output](char16_t const* labelText) {
    return AppBarButton {
        label = labelText,
        onClick += [output, labelText](AppBarButton const&) { output.text(std::u16string {u"You clicked: "} + labelText); },
    };
};

auto commandFlyout = CommandBarFlyout {
    placement = FlyoutPlacementMode::Right,
    primaryCommands[makeCommand(Symbol::Share, u"Share"), makeCommand(Symbol::Save, u"Save"), makeCommand(Symbol::Delete, u"Delete")],
    secondaryCommands[plain(u"Resize"), plain(u"Move")],
};

// Standard: the focus moves to the flyout. Transient: it stays where it was.
auto show = [commandFlyout](DependencyObject const& anchor, FlyoutShowMode how) {
    commandFlyout.showAt(anchor, FlyoutShowOptions {showMode = how, placement = FlyoutPlacementMode::RightEdgeAlignedTop});
};

auto picture = Button {
    Margin {0, 12},
    Padding {0},
    toolTip = u"mountain",
    content = Image {height = 300, source = u"Assets/SampleMedia/rainier.jpg"},
    onClick += [show](Button const& self) { show(self, FlyoutShowMode::Transient); },
    onContextRequested += [show](Button const& self) { show(self, FlyoutShowMode::Standard); },
};

auto example = StackPanel {
    TextBlock {u"Click or right click the image to open a CommandBarFlyout"},
    picture,
    output,
};