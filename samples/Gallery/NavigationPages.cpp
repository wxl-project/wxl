// Страницы для рамок образцов навигации.

#include "NavigationPages.h"

#include "Box.h"
#include "IndexList.h"
#include "Pages.h"
#include <wxl/Microsoft.UI.Xaml.Media.h>

using namespace wxl;

void gallery::showSample(Frame const& frame, int number) {
    navigatePage(frame).content(samplePage(number));
}

void gallery::showSample(Frame const& frame, int number, NavigationTransitionInfo const& info) {
    navigatePage(frame, info).content(samplePage(number));
}

void gallery::showSettings(Frame const& frame) {
    navigatePage(frame).content(sampleSettingsPage());
}

int gallery::sampleNumber(Object const& tag) {
    auto const text = unboxValue<hstring>(tag);
    return text.empty() ? 1 : text.data()[text.size() - 1] - u'0';
}

std::u16string gallery::sampleHeader(int number) {
    return u"Sample Page " + std::u16string{static_cast<char16_t>(u'0' + number)};
}

MenuFlyout gallery::tabViewContextMenu() {
    using namespace wxl::dsl;
    return MenuFlyout {onOpening += [](MenuFlyout const& self) {
        populateTabViewContextMenu(self);
        // A menu that ended up with nothing in it is not shown.
        if (self.items().size() == 0) {
            self.hide();
        }
    }};
}

void gallery::populateTabViewContextMenu(MenuFlyout const& flyout) {
    using namespace wxl::dsl;
    flyout.items().clear();

    auto const anchor = flyout.target();
    if (!anchor || !anchor.is<TabViewItem>()) {
        return;
    }
    auto const item = anchor.try_as<TabViewItem>();

    // Up the tree from the tab: the list that holds the tabs and the TabView around it.
    core::nullable<ListView> list;
    core::nullable<TabView> tabs;
    DependencyObject current = item;
    while (current) {
        auto const parent = VisualTreeHelper::getParent(current);
        if (!parent) {
            break;
        }
        if (parent.is<ListView>()) {
            list = parent.try_as<ListView>();
        } else if (parent.is<TabView>()) {
            tabs = parent.try_as<TabView>();
        }
        if (list && tabs) {
            break;
        }
        current = parent;
    }
    if (!list || !tabs) {
        return;
    }

    // A TabView has either tab items of its own or a source of data; a tab moves by taking the item out of whichever it is and
    // putting it back one place on.
    auto const view = *tabs;
    auto const index = static_cast<uint32_t>((*list).indexFromContainer(item));
    auto const move = [view](uint32_t oldIndex, uint32_t newIndex) {
        if (auto const boundSource = view.tabItemsSource()) {
            IndexList const list{boundSource};
            auto const moved = list.getAt(oldIndex);
            list.removeAt(oldIndex);
            list.insertAt(newIndex, moved);
        } else {
            auto const tabs = view.tabItems();
            auto const moved = tabs[oldIndex];
            tabs.removeAt(oldIndex);
            tabs.insertAt(newIndex, moved);
        }
    };

    if (index > 0) {
        flyout.items().append(MenuFlyoutItem {text = u"Move tab left", onClick += [move, index](auto&&...) { move(index, index - 1); }});
    }
    if (index + 1 < (*list).items().size()) {
        flyout.items().append(MenuFlyoutItem {text = u"Move tab right", onClick += [move, index](auto&&...) { move(index, index + 1); }});
    }
}
