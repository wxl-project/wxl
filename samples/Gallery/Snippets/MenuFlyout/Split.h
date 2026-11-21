auto output = TextBlock {Margin {8, 0, 0, 0}, vAlign.center};

auto item = [output](char16_t const* labelText) {
    return MenuFlyoutItem {
        text = labelText,
        onClick += [output, labelText](MenuFlyoutItem const&) { output.text(std::u16string {u"Clicked: "} + labelText); },
    };
};

auto button = Button {
    content = u"File Options",
    flyout = MenuFlyout {
        SplitMenuFlyoutItem {
            text = u"Save",
            icon = FontIcon {glyph = u"\uE74E"},
            onClick += [output](MenuFlyoutItem const&) { output.text(u"Clicked: Save"); },
            item(u"Save as .docx"),
            item(u"Save as .pdf"),
            item(u"Save as .txt"),
        },
        SplitMenuFlyoutItem {
            text = u"Share",
            icon = SymbolIcon {symbol = Symbol::Share},
            onClick += [output](MenuFlyoutItem const&) { output.text(u"Clicked: Share"); },
            item(u"Share via email"),
            item(u"Share via link"),
        },
    },
};