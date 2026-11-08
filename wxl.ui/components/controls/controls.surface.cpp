// Compile-time check of the declarative surface of the hand-written controls,
// with the lines of their schema entries (controls.schema.h).
// Built into the surface test of every profile that selects the component;
// nothing here runs.

#include "Card.h"
#include "ui.h"
#include "schema.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// The library looks are Borders of their own, and the look goes on ahead of
// the caller's setters, so anything in it is a default the page can argue
// with -- here the face, for a card that lies over artwork and must not
// drown it. A card passes wherever its base does, and one written with a
// single child claims the positional route Border already has.
[[maybe_unused]] void library_looks(UIElement const& content) {
    Card plain{content};
    OverlayCard over_picture{content};

    Card over_artwork{
        hAlign.right,
        vAlign.top,
        Margin{0, 64, 72, 0},
        background = SolidColorBrush{rgba(28, 18, 8, 0.424)},
        content,
    };

    // The copy guard in setter_pack: one argument of the look's own type is
    // a copy, not a setter pack of one.
    Card const copied{plain};
    Border const& asBorder = copied;
    static_assert(std::derived_from<Card, Border>);
    (void)asBorder;

    // And a look is storable, not merely spendable: a screen keeps the card
    // it built the way it keeps any other control. The wrapper sentinel
    // builds the empty state through a maker derived from the look, which
    // hands its own base a null Impl -- so this line is the whole test of
    // the Impl constructor a hand-written control needs.
    core::nullable<OverlayCard> kept = nullptr;
    kept = over_picture;
    core::nullable<Border> const widened = kept;
    (void)widened;
}

// A hand-written wrapper is read from a sender like any generated control.
// EventHandler adapts a handler that names the concrete type, and it does so
// through try_as, which builds the wrapper from an Impl -- so the protected
// constructor these two declare is what this compiles into existence. A
// handler that could not name its own type would have to capture the control,
// which is the one thing a wxl handler must not do.
[[maybe_unused]] void hand_written_looks_as_senders() {
    Card{}.add_onActualThemeChanged([](Card const& self, Object const&) { self.padding({8}); });
    OverlayCard{}.add_onActualThemeChanged(
        [](OverlayCard const& self, Object const&) { self.padding({8}); });
}

// SettingsCard -- written by hand, so its schema lines are too.
[[maybe_unused]] void SettingsCard_header_assigned(::wxl::SettingsCard const& object,
                                                   ::wxl::Object const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsCard::header = value);
}
[[maybe_unused]] void SettingsCard_description_assigned(::wxl::SettingsCard const& object,
                                                        ::wxl::hstring const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsCard::description = value);
}
[[maybe_unused]] void SettingsCard_headerIcon_assigned(::wxl::SettingsCard const& object,
                                                       ::wxl::IconElement const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsCard::headerIcon = value);
}
[[maybe_unused]] void SettingsCard_actionIcon_assigned(::wxl::SettingsCard const& object,
                                                       ::wxl::IconElement const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsCard::actionIcon = value);
}
[[maybe_unused]] void SettingsCard_isClickEnabled_assigned(::wxl::SettingsCard const& object,
                                                           bool value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsCard::isClickEnabled = value);
}
[[maybe_unused]] void SettingsCard_isActionIconVisible_assigned(::wxl::SettingsCard const& object,
                                                                bool value) {
    ::wxl::impl::apply_argument(object,
                                ::wxl::dsl::schema::SettingsCard::isActionIconVisible = value);
}
[[maybe_unused]] void SettingsCard_contentAlignment_assigned(
    ::wxl::SettingsCard const& object, ::wxl::SettingsCardContentAlignment value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsCard::contentAlignment = value);
}
// The braces as the settings page writes them: the named members, the setting as the unnamed child, a click.
[[maybe_unused]] void SettingsCard_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    SettingsCard{header = TextBlock{u"App theme"},
                 description = u"Select which app theme to display",
                 headerIcon = FontIcon{glyph = u"x"},
                 isClickEnabled = true,
                 contentAlignment = SettingsCardContentAlignment::Vertical,
                 onClick += [](Object const&, RoutedEventArgs&) {},
                 Button{u"Setting"}};
}

// SettingsExpander -- written by hand, so its schema lines are too.
[[maybe_unused]] void SettingsExpander_header_assigned(::wxl::SettingsExpander const& object,
                                                       ::wxl::Object const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsExpander::header = value);
}
[[maybe_unused]] void SettingsExpander_description_assigned(::wxl::SettingsExpander const& object,
                                                            ::wxl::hstring const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsExpander::description = value);
}
[[maybe_unused]] void SettingsExpander_headerIcon_assigned(::wxl::SettingsExpander const& object,
                                                           ::wxl::IconElement const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsExpander::headerIcon = value);
}
[[maybe_unused]] void SettingsExpander_isExpanded_assigned(::wxl::SettingsExpander const& object,
                                                           bool value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::SettingsExpander::isExpanded = value);
}
// The braces as the settings page writes them: the head, its setting as the unnamed child, the cards of the list.
[[maybe_unused]] void SettingsExpander_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    SettingsExpander{header = TextBlock{u"Sound"},
                     description = u"Audible feedback",
                     isExpanded = true,
                     onExpanding += [](Object const&, Object const&) {},
                     ToggleSwitch{},
                     items[SettingsCard{header = TextBlock{u"Spatial audio"}}]};
}

// HeaderedContentControl -- written by hand, so its schema line is too.
[[maybe_unused]] void HeaderedContentControl_header_assigned(
    ::wxl::HeaderedContentControl const& object, ::wxl::Object const& value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HeaderedContentControl::header = value);
}
[[maybe_unused]] void HeaderedContentControl_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    HeaderedContentControl{header = TextBlock{u"Name"}, TextBox{}};
}

}  // namespace
