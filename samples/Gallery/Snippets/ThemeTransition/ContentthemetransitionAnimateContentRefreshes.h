auto const list = ListView {itemContainerTransitions[ContentThemeTransition {}]};

// Five items; "Refresh data" puts in their place five others, and the list shows them with the transition.
auto const refill = [list](bool updated) {
    std::vector<std::u16string> lines;
    for (int i = 0; i < 5; ++i) {
        lines.push_back((updated ? u"Updated content " : u"Item ") + gallery::numberText(i));
    }
    list.itemsSource(stringList(lines));
};
refill(false);

auto example = Grid {list};
auto options = StackPanel {Button {content = u"Refresh data", onClick += [refill](auto&&...) { refill(true); }}};