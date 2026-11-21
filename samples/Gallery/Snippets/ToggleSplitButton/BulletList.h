auto const marker = std::make_shared<MarkerType>(MarkerType::Bullet);

auto richBox = RichEditBox {width = 240, minHeight = 96, automationName = u"Text entry"};

auto markerIcon = SymbolIcon {symbol = Symbol::List};

const Preset choice {
    Padding {4},
    minWidth = 0,
    minHeight = 0,
    Margin {6},
    CornerRadius {4},
};

// One coroutine per choice: it waits for its click, applies its marker and
// closes the flyout. The button the flyout belongs to is found through the
// flyout's Target, so nothing captures it. The page's model keeps them, and
// the next opening of the flyout replaces them.
struct Model {
    std::vector<async::task<>> choosing;
};
auto const model = gallery::hold<Model>();

auto const choose = [](Flyout popup, Button button, Symbol markerSymbol, MarkerType type, SymbolIcon markerIcon,
                       RichEditBox richBox, std::shared_ptr<MarkerType> marker) -> async::task<> {
    auto clicks = onClick(button);
    if (co_await clicks.next()) {
        *marker = type;
        markerIcon.symbol(markerSymbol);
        richBox.document().selection().paragraphFormat().listType(type);
        popup.target().try_as<ToggleSplitButton>().isChecked(true);
        popup.hide();
        richBox.focus(FocusState::Keyboard);
    }
};

auto listButton = ToggleSplitButton {
    vAlign.top,
    automationName = u"Bullets",
    content = markerIcon,
    onIsCheckedChanged += [richBox, marker](ToggleSplitButton const& self) {
        richBox.document().selection().paragraphFormat().listType(
            self.isChecked() ? *marker : MarkerType::None);
    },
    flyout = Flyout {
        placement = FlyoutPlacementMode::Bottom,
        onOpened += [model, choose, richBox, markerIcon, marker](Object const& sender) {
            auto const popup = sender.try_as<Flyout>();
            auto const buttons = popup.content().try_as<StackPanel>().children();
            model->choosing.clear();
            model->choosing.push_back(
                choose(popup, buttons[0].try_as<Button>(), Symbol::List, MarkerType::Bullet, markerIcon, richBox, marker));
            model->choosing.push_back(choose(popup, buttons[1].try_as<Button>(), Symbol::Bullets,
                                             MarkerType::UppercaseRoman, markerIcon, richBox, marker));
        },
        content = StackPanel {
            orientation.horizontal,
            Button {choice, automationName = u"Bulleted list", content = SymbolIcon {symbol = Symbol::List}},
            Button {choice, automationName = u"Roman numerals list", content = SymbolIcon {symbol = Symbol::Bullets}},
        },
    },
};