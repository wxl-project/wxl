auto output = TextBlock {};

// Every item says what it is into the line above the bar.
auto item = [output](char16_t const* labelText) {
    return MenuFlyoutItem {
        text = labelText,
        onClick += [output, labelText](MenuFlyoutItem const&) { output.text(std::u16string {u"You clicked: "} + labelText); },
    };
};

auto bar = MenuBar {
    MenuBarItem {title = u"File", item(u"New"), item(u"Open"), item(u"Save"), item(u"Exit")},
    MenuBarItem {title = u"Edit", item(u"Undo"), item(u"Cut"), item(u"Copy"), item(u"Paste")},
    MenuBarItem {title = u"Help", item(u"About")},
};