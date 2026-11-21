auto const menu = gallery::tabViewContextMenu();
auto const programTab = [menu](char16_t const* tabTitle, char16_t const* iconPath) {
    return TabViewItem {header = tabTitle, isClosable = false, contextFlyout = menu,
                        iconSource = BitmapIconSource {showAsMonochrome = false, uriSource = iconPath}};
};

auto example = StackPanel {
    TextBlock {Margin {0, 0, 0, 12}, textWrapping = TextWrapping::WrapWholeWords,
               u"Use BitmapIcon.ShowAsMonochrome=\"False\" to display full color icons in the TabViewItem"},
    TabView {
        onBringIntoViewRequested += [](auto const&, BringIntoViewRequestedEventArgs& args) { args.handled(true); },
        minWidth = 490,
        isAddTabButtonVisible = false,
        selectedIndex = 0,
        tabWidthMode = TabViewWidthMode::SizeToContent,
        tabItems[programTab(u"CMD Prompt", u"Assets/SampleMedia/cmd.png"), programTab(u"PowerShell", u"Assets/SampleMedia/powershell.png"),
                 programTab(u"Windows Subsystem for Linux", u"Assets/SampleMedia/linux.png")],
    },
};