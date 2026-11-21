#pragma once

// What stands behind the properties of lists and item collections WinUI writes as templates of
// XAML text, or as collections with a property of their own.
//
// Private: the control arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include "core.h"
#include "ItemTemplate.h"
#include "ItemContainerPreset.h"
#include "Thickness.h"
#include "impl/item_makers.h"
#include <wxl/Microsoft.UI.Xaml.Controls.Enums.h>

namespace wxl::impl {

// `itemTemplate = [](Object const& item) {...}` and what its containers wear on a ListView or a
// GridView: the parts of the list's state that impl/item_makers.h keeps, given as the wrappers
// write them. Defined in item_template.cpp.

/// The function that makes the element of each item, called with the item as an Object.
void set_item_template(winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
                       ItemBuilder const& build);

/// The margin of every container of the list, the ones on screen and the ones made later -- what
/// an ItemContainerStyle with a Setter for Margin says.
void set_item_margin(winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
                     Thickness const& margin);

/// The preset every container of the list wears, once, the first time the control prepares it
/// (ContainerContentChanging) -- what an ItemContainerStyle says. The containers on screen already
/// are dressed here and now.
void set_item_container_style(winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
                              ItemContainerPreset const& preset);

// `itemTemplate = [](Object const& item) {...}` on an ItemsRepeater or an ItemsView: the element
// factory of impl/item_makers.h over the function, the way CustomLayout is a layout class with its
// functions handed in. Defined in item_factory.cpp.

void set_repeater_template(winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater const& repeater,
                           ItemBuilder const& build);
void set_items_view_template(winrt::Microsoft::UI::Xaml::Controls::ItemsView const& view,
                             ItemBuilder const& build);

/// `itemsPanelOrientation = Orientation::Vertical` on an ItemsControl. An ItemsPanelTemplate is
/// made from XAML text only, so the shortest text there is -- a VirtualizingStackPanel -- is run
/// through the framework's own loader, and the control gets the template a XAML file would give.
void set_items_panel_orientation(winrt::Microsoft::UI::Xaml::Controls::ItemsControl const& control,
                                 Orientation orientation);

/// `elementItems = true` on a TreeView: its items are elements, shown as the elements they are.
/// With no ItemTemplate a TreeView writes the name of the type, and a DataTemplate is made from
/// XAML text only: a ContentControl that shows the content of its node.
void set_element_items(winrt::Microsoft::UI::Xaml::Controls::TreeView const& control, bool on);

// `leftItems` and `rightItems` having a mode. SwipeControl.LeftItems is a SwipeItems, a vector of
// SwipeItem with a Mode besides; wxl stands every such vector class for the collection itself, and
// a collection has no mode, so the mode is a property of the control and is set on its items.

void set_left_items_mode(winrt::Microsoft::UI::Xaml::Controls::SwipeControl const& control,
                         SwipeMode mode);

void set_right_items_mode(winrt::Microsoft::UI::Xaml::Controls::SwipeControl const& control,
                          SwipeMode mode);

}  // namespace wxl::impl
