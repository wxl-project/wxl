auto const menu = gallery::tabViewContextMenu();
auto const fixedTab = [menu](char16_t const* tabTitle, Symbol tabSymbol, int page) {
    return TabViewItem {header = tabTitle, isClosable = false, contextFlyout = menu, iconSource = SymbolIconSource {tabSymbol = tabSymbol},
                        content = gallery::samplePage(page)};
};

auto const tabs = TabView {
    onBringIntoViewRequested += [](auto const&, BringIntoViewRequestedEventArgs& args) { args.handled(true); },
    minHeight = 475,
    Margin {-12},
    isAddTabButtonVisible = false,
    selectedIndex = 0,
    tabWidthMode = TabViewWidthMode::SizeToContent,
    tabItems[fixedTab(u"Home", Symbol::Home, 1), fixedTab(u"Tab 2 Has Longer Text", Symbol::MusicInfo, 2), fixedTab(u"Third Tab", Symbol::Placeholder, 3)],
};

auto example = tabs;
auto options = ComboBox {
    width = 150,
    header = u"TabWidthBehavior",
    ComboBoxItem {content = u"SizeToContent"},
    ComboBoxItem {content = u"Equal"},
    ComboBoxItem {content = u"Compact"},
    selectedIndex = 0,
    onSelectionChanged += [tabs](ComboBox const& self) {
        static constexpr TabViewWidthMode modes[] = {TabViewWidthMode::SizeToContent, TabViewWidthMode::Equal, TabViewWidthMode::Compact};
        tabs.tabWidthMode(modes[std::max(self.selectedIndex(), 0)]);
    },
};