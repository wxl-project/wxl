#pragma once

// What makes the element of each item of a list control, over the projection alone: the state a
// ListView or GridView keeps for its containers, the element factory of an ItemsView or an
// ItemsRepeater, and the bound list (`itemsSource = BindOutput{list, build}`) given to each of
// the three. A profile reaches these with the control it holds: the item template members of the
// profile's wrappers, and the generated attach_items of each list control the profile has
// (BindingPairs.h), hand over the projection object the wrapper already keeps.
//
// One header for the area and two sources split by the kind of control: item_template.cpp for the
// controls that prepare containers, item_factory.cpp for those that ask a factory.
//
// Private: this header names winrt types, so only wxl's own sources include it.

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include "../core.h"

namespace wxl::impl {

class bound_list_watch;

/// What makes an element for an item, both as the platform hands them over: an item of
/// ItemsSource in, the element to show it out.
using winrt_item_builder = core::function<winrt::Windows::Foundation::IInspectable(
    winrt::Windows::Foundation::IInspectable const& item)>;

/// What a container of a list wears once, the first time the control prepares it.
using container_dress = core::function<void(
    winrt::Microsoft::UI::Xaml::Controls::Primitives::SelectorItem const& container)>;

// A ListView or a GridView makes a container for an item when it comes into view and lets the
// application fill it (ContainerContentChanging): the container shows the element the function made
// for the item, and is cleared when it goes to the recycle queue -- an ItemTemplate with the
// function in place of the template. The function runs inside a binding_scope the container keeps
// (impl/element_scope.h), so the bindings of an element go when the container gives it back. The
// state of a list -- the function, the margin and the dress of its containers -- is held by the
// list, through its one handler, and each setter replaces its own part of it. Defined in
// item_template.cpp.

/// The function that makes the element of each item, and the template that gives each container
/// a place for it; the containers on screen are filled again.
void set_item_maker(winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
                    winrt_item_builder make);

/// The margin of every container of the list, the ones on screen and the ones made later.
void set_container_margin(winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
                          winrt::Microsoft::UI::Xaml::Thickness const& margin);

/// What every container of the list wears, once; the containers on screen are dressed now.
void set_container_dress(winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
                         container_dress dress);

/// The element the maker of the list shows for `item`, or null when the item has no container in
/// view or its container shows no element.
winrt::Microsoft::UI::Xaml::UIElement element_of_item(
    winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
    winrt::Windows::Foundation::IInspectable const& item);

// An ItemsRepeater or an ItemsView makes an element for an item through a factory
// (IElementFactory); this one is a COM object of wxl's own (impl/sta_com.h) with the function
// handed in. The function runs inside a binding_scope the factory keeps for the element; an
// element given back (RecycleElement) has its bindings unbound at once. Defined in
// item_factory.cpp.

/// The factory of elements made by `make`. `containers`: the factory of an ItemsView, which takes
/// an ItemContainer for each item and nothing else.
winrt::Microsoft::UI::Xaml::IElementFactory element_factory_of(winrt_item_builder make,
                                                               bool containers);

// `itemsSource = BindOutput{list, build}`: the source of the bound list as the control's
// ItemsSource, and the maker that asks it for the element of each item. The maker first, so that
// the first elements the source brings are made by it.

void attach_list_view_items(winrt::Microsoft::UI::Xaml::Controls::ListViewBase const& list,
                            bound_list_watch const& source);
void attach_items_view_items(winrt::Microsoft::UI::Xaml::Controls::ItemsView const& view,
                             bound_list_watch const& source);
void attach_repeater_items(winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater const& repeater,
                           bound_list_watch const& source);

}  // namespace wxl::impl
