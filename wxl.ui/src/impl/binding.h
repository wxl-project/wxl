#pragma once

// The binding pairs, the winrt-free side: a control property <-> an observable
// field, both ways or the one asked for.
//
// A pair is a property the control writes itself, with the events it says so
// under. The generator writes one per pair the type map names
// (wxl.gen/profiles/types.json, "bindingPairs") for every class the profile
// generates, as impl::binding_pair<key, class> in <wxl/BindingPairs.h>, and
// each of them is bind_pair below over the wrapper's own getter, setter and
// events. A property the control does not write has no pair: it is bound one
// way, from the field, by the setter the generator already emits
// (bind_property in member.h).
//
// A pair runs both ways unless `direction` keeps one: input is the control ->
// field half alone, output the field -> control half alone. A pair whose own
// direction is input has that half only: the control reports the property and
// never shows it, so it takes BindInput and nothing else.
//
// Three pairs are functions of their own, each in a file of its own that
// reaches only the wrapper of its class: isFocused (focus_pair.h),
// verticalOffset (vertical_offset_pair.h) and a NumberBox's intermediateValue
// (intermediate_value_pair.h). The type map names each with its function and
// that header ("boundMembers"), and the generated pair calls it. Two of them
// bind state the control takes only on its own terms -- isFocused and a
// ScrollViewer's verticalOffset (impl/state_pair.h). They have no setter: the
// field asks, the control does what it can, and the field says what it did. So
// they take Bind or BindInput, never BindOutput, which would leave the field
// saying what was asked.

#include "core.h"

#include "../events.h"
#include "member.h"

namespace wxl::impl {

// The control-side half of a binding pair, and the reason its handlers may
// hold the field by bare address. The revoker lives inside the field's own
// watch (bind_pair below), so it goes when the watch does -- with the field,
// or through unbind() -- and takes the handlers off the control first.
// Nothing is ever left on a control that could write to a field that is gone.
//
// It owns the control, as the watch it lives in has to anyway: a control is a
// temporary in the description tree, and the watch writes to it. One token
// per event, each empty where nothing was added.
template <class Control, EventKey... keys>
struct handler_revoker {
    Control control;
    std::array<EventToken, sizeof...(keys)> tokens;

    handler_revoker(Control const& c, std::array<EventToken, sizeof...(keys)> t)
        : control(c), tokens(std::move(t)) {}

    // Moved into the watch's closure once; a token's own move leaves the
    // source with nothing to remove, so that only one of the two ever does.
    handler_revoker(handler_revoker&& other) noexcept
        : control(std::move(other.control)), tokens(std::move(other.tokens)) {}

    handler_revoker(handler_revoker const&) = delete;
    handler_revoker& operator=(handler_revoker const&) = delete;
    handler_revoker& operator=(handler_revoker&&) = delete;

    ~handler_revoker() {
        [this]<std::size_t... at>(std::index_sequence<at...>) {
            ((tokens[at] ? static_cast<void>(event_adder<keys>::remove(control, tokens[at]))
                         : static_cast<void>(0)),
             ...);
        }(std::make_index_sequence<sizeof...(keys)>{});
    }
};

// The shape shared by every binding pair, whatever the control and the value:
// the events that say the control wrote the property, and two little
// operations for how the property is read and written.
//
//  - `set` writes the field's value into the control. It runs once at the start
//    so the control opens showing the value, and again on every change.
//  - `get` reads the value back out for the control -> field direction, under
//    each of the events: a checked box says so under Checked and Unchecked.
//
// Asked for one direction, the pair keeps that half alone: input adds the
// handlers and writes the control never, output writes the control and hears
// it never. The watch stays either way, with nothing to write for input, since
// it is what holds the revoker.
//
// The control -> field handlers name the control as their sender and the
// field by address, so they hold neither: the sender is read rather than
// captured, and the address is good for as long as the handlers exist, which
// the revoker sees to. The field -> control watch owns the control, through
// the revoker. So the ownership runs one way -- model -> watch -> control --
// and there is no cycle to break: the model goes, its watches go, the
// handlers come off, the control is let go. Only a model that outlives its
// window needs unbind(), and then the revoker does the same on the way out.
//
// No re-entry guard: writing the control raises its change event, which
// writes the same value back, and observable::set says nothing when nothing
// changed. NaN, a NumberBox's word for empty, equals nothing, so set cannot end
// that echo; the box raises no ValueChanged for NaN over NaN, which does.
template <EventKey... keys, class Control, class T, class Get, class Set>
void bind_pair(Control const& control, core::observable<T>& field, bind_direction direction,
               Get get, Set set) {
    bool const shows = direction != bind_direction::input;
    bool const edits = direction != bind_direction::output;

    if (shows) {
        set(control, field.get());
    } else if (edits) {
        // Input alone: the field takes what the control has right now, so the
        // two agree from the first moment and the first change is a change.
        field.set(get(control));
    }

    auto const listen = [&]<EventKey key>() {
        using Args = typename event_adder<key>::template args_t<Control>;
        return event_adder<key>::add(control, [&field, get](Control const& sender, EventArgsRef<Args>) {
            field.set(get(sender));
        });
    };
    handler_revoker<Control, keys...> revoker{
        control, [&]() -> std::array<EventToken, sizeof...(keys)> {
            if (!edits) return {};
            return {listen.template operator()<keys>()...};
        }()};

    if (shows) {
        field.watch_for_binding([revoker = std::move(revoker), set](T const& value) noexcept {
            set(revoker.control, value);
        });
    } else {
        field.watch_for_binding([revoker = std::move(revoker)](T const&) noexcept {});
    }
}

}  // namespace wxl::impl
