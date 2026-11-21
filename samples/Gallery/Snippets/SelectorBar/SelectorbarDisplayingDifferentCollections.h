// Three collections of tiles: the items are the numbers of the tiles, the hundreds say which collection a tile belongs to.
constexpr Color pink = rgb(255, 192, 203);
constexpr Color plum = rgb(221, 160, 221);
constexpr Color powderBlue = rgb(176, 224, 230);
static constexpr int counts[] = {5, 7, 4};

auto const view = ItemsView {
    layout = StackLayout {orientation.horizontal},
    itemTemplate = [](Object const& item) {
        auto const category = unboxValue<int64_t>(item) / 100;
        return ItemContainer {width = 112, height = 82, Margin {4}, background = category == 0 ? pink : category == 1 ? plum : powderBlue};
    },
};
auto const show = [view](int category) {
    std::vector<int64_t> tiles;
    for (int i = 0; i < counts[category]; ++i) {
        tiles.push_back(category * 100 + i);
    }
    view.itemsSource(IndexList{tiles});
};
show(0);

auto const bar = SelectorBar {
    items[SelectorBarItem {text = u"Pink", isSelected = true}, SelectorBarItem {text = u"Plum"}, SelectorBarItem {text = u"PowderBlue"}],
    onSelectionChanged += [show](SelectorBar const& sender, SelectorBarSelectionChangedEventArgs&) {
        auto const selected = sender.selectedItem().text();
        show(selected == u"Pink" ? 0 : selected == u"Plum" ? 1 : 2);
    },
};

auto example = StackPanel {bar, view};