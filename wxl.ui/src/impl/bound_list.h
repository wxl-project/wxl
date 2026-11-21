#pragma once

// The application's side of `itemsSource = BindOutput{list, build}`: what the binding does
// to a list control, with no winrt in sight.
//
// The control is given a source of wxl's own (impl::bound_items_source): a WinRT vector
// whose items are slots, one per item of the list, each knowing its place. The list's
// watch mirrors every change of the list into the source, which tells the control. The
// control asks for the element of an item when the item comes into view, and the machinery
// behind the control's item template (impl/item_makers.h) asks the
// slot's source, which calls `build` with the item -- inside a binding_scope the machinery
// keeps until the control gives the element back.
//
// Who owns what. The list owns its watch; the watch owns the source (bound_list_watch) and,
// going, disarms it: the source forgets the list and the builder, so a control that
// outlives the list builds nothing from it. The control owns the source too, as its
// ItemsSource. Nothing holds the control but its parent.

#include "../Object.h"

namespace wxl {
class UIElement;
class ItemContainer;
class ItemsView;
}  // namespace wxl

namespace wxl::impl {

class bound_items_source;

/// The source one binding of a list gives its control, as the list's watch holds it: one
/// reference, moved and never copied. It is the watch itself -- called with each change of
/// the list, it mirrors the change into the source -- and when the watch goes it disarms
/// the source before letting go of it.
class bound_list_watch
{
public:
    /// The element of the item at a position, built by the application's function.
    using indexed_item_builder = core::function<Object(uint32_t at)>;

    /// A source of `size` items over the list at `list` -- its address, by which boundItem
    /// knows a box of this list from one of another.
    bound_list_watch(void const* list, uint32_t size, indexed_item_builder build);

    bound_list_watch(bound_list_watch&& other) noexcept;
    bound_list_watch(bound_list_watch const&) = delete;
    bound_list_watch& operator=(bound_list_watch const&) = delete;
    bound_list_watch& operator=(bound_list_watch&&) = delete;
    ~bound_list_watch();

    /// One change of the list, mirrored into the source and told to the control.
    void operator()(core::list_change const& change) const noexcept;

    bound_items_source& source() const noexcept { return *source_; }

private:
    bound_items_source* source_;
};

/// Where the item a control handed out (clickedItem, selectedItem) stands in the list at
/// `list`, or -1: the box is no slot, a slot of another list, or of an item that is gone.
int64_t bound_position(void const* list, Object const& box) noexcept;

/// A control a list binds to: one with an `attach_items(control, source)`, which gives it the
/// source and the machinery of its elements -- its ItemsSource, and the item template that asks
/// the source for the element of each item. The generator declares one for each list control of
/// the profile (BindingPairs.h) and defines it over the wrapper's own object and
/// impl/item_makers.h, so a bound list needs no component of the profile.
template <class Control>
concept bound_items_control = requires(Control const& control, bound_list_watch const& source) {
    attach_items(control, source);
};

/// `itemsSource = BindOutput{list, build}`, applied to the control being built.
template <class Control, class T, class Build>
void bind_items(Control const& control, core::observable_list<T const>& list, Build const& build) {
    static_assert(bound_items_control<Control>,
                  "wxl: a list is bound to the items of a ListView, a GridView, an ItemsView or an "
                  "ItemsRepeater.");
    static_assert(std::is_invocable_v<Build const&, T const&>,
                  "wxl: the function of BindOutput{list, fn} builds the element of one item: it "
                  "takes the item, "
                  "`[](T const& item) { return ...; }`.");
    using element_t = std::remove_cvref_t<std::invoke_result_t<Build const&, T const&>>;
    static_assert(
        std::derived_from<element_t, UIElement>,
        "wxl: the function of BindOutput{list, fn} returns the element that shows the item.");
    if constexpr (std::derived_from<Control, ItemsView>) {
        static_assert(
            std::derived_from<element_t, ItemContainer>,
            "wxl: an ItemsView shows each item in an ItemContainer: the function returns one, "
            "`ItemContainer {child = ...}`.");
    }

    bound_list_watch watch{&list, list.size(),
                           [&list, build](uint32_t at) -> Object { return std::invoke(build, list[at]); }};
    attach_items(control, watch);
    list.watch_for_binding(std::move(watch));
}

}  // namespace wxl::impl
