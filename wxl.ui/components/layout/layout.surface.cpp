// Compile-time check of the declarative surface of the layouts and panels,
// with the lines of their schema entries (layout.schema.h).
// Built into the surface test of every profile that selects the component,
// beside test/dsl_surface.cpp; nothing here runs.

#include "Bind.h"
#include "CustomLayout.h"
#include "Panels.h"
#include "SplitPanel.h"
#include "ui.h"
#include "schema.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// A grid's rows and columns written as text are parsed when the code compiles into a static array
// of GridLength, and RowDefinitions<...> / ColumnDefinitions<...> are views on it: written unnamed in
// the Grid's braces, kept by a Preset with nothing that can go away, and a choice between two texts
// is a choice between two values of one type. Lengths counted while the program runs go to the
// named tag as GridLength values -- a span or a braced list -- borrowed for the expression that
// writes them, which is why a Preset and a Template refuse them (borrows_items, as a collection list).
[[maybe_unused]] void grid_lengths(bool iconAfter, int columns) {
    Grid literal{RowDefinitions<u"auto, 2*,*,120">, ColumnDefinitions<u"*,1.5*">};
    Grid chosen{iconAfter ? ColumnDefinitions<u"*,auto"> : ColumnDefinitions<u"auto,*">};
    constexpr auto pinned = ColumnDefinitions<u"*,*,*">;
    Grid fixed{pinned};
    Apply{fixed, RowDefinitions<u"auto">};

    std::vector<GridLength> const counted(static_cast<std::size_t>(columns),
                                          GridLength{1.0, GridUnitType::Star});
    Grid computed{columnDefinitions = counted};
    Grid listed{rowDefinitions = {GridLength{1.0, GridUnitType::Auto},
                                  GridLength{2.0, GridUnitType::Star}}};
    Apply{computed, rowDefinitions = std::span<GridLength const>{counted}};

    static constexpr Preset look{RowDefinitions<u"auto,*">};
    Grid dressed{look};

    static_assert(pinned.size() == 3);
    static_assert(RowDefinitions<u"auto, 2*,*,120">.size() == 4);
    constexpr GridLength shares = ColumnDefinitions<u" 1.5* ">.lengths()[0];
    static_assert(shares.value == 1.5 && shares.gridUnitType == GridUnitType::Star);
    static_assert(!std::is_same_v<RowLengths, ColumnLengths>);
    static_assert(!impl::borrows_items<std::decay_t<decltype(RowDefinitions<u"*">)>>);
    static_assert(impl::borrows_items<decltype(rowDefinitions = std::span<GridLength const>{})>);
}

// A text that is not lengths does not compile. Asked of the parse itself -- the one that makes
// RowDefinitions and ColumnDefinitions -- through a template argument, so that the evaluation that
// fails is the answer of a requires-expression rather than an error on this line.
template <impl::values Text>
concept parses_as_grid_lengths = requires {
    typename std::integral_constant<std::size_t, impl::grid_lengths_parse<Text>().size()>;
};

static_assert(parses_as_grid_lengths<u"*,2*,auto,120">);
static_assert(!parses_as_grid_lengths<u"*,2x,auto">);
static_assert(!parses_as_grid_lengths<u"*,,auto">);
static_assert(!parses_as_grid_lengths<u"">);

// A fraction is the double nearest what is written, as a literal of the language is.
constexpr auto grid_fractions = RowDefinitions<u"0.3,0.7*,1.25">.lengths();
static_assert(grid_fractions[0].value == 0.3);
static_assert(grid_fractions[1].value == 0.7);
static_assert(grid_fractions[2].value == 1.25);
static_assert(!parses_as_grid_lengths<u"12345678901234567">);
static_assert(!parses_as_grid_lengths<u"0.00000000000000000000001">);

// Only a string literal is a text. A lone character, characters with no terminator and numbers make
// a values too, and the parse refuses them before it reads an entry.
static constexpr char16_t grid_unterminated[]{u'*', u',', u'*'};
static constexpr int grid_numbers[]{1, 2};
static_assert(!parses_as_grid_lengths<'*'>);
static_assert(!parses_as_grid_lengths<u'*'>);
static_assert(!parses_as_grid_lengths<grid_unterminated>);
static_assert(!parses_as_grid_lengths<42>);
static_assert(!parses_as_grid_lengths<grid_numbers>);

// Written in a default member initializer, as a window's model writes its grid: the check of the
// text has to hold there too.
struct [[maybe_unused]] grid_in_a_model {
    Grid rows{RowDefinitions<u"32,*">};
    Grid columns{ColumnDefinitions<u"*,auto">};
};

// The even stack: children claim a star row (column) apiece instead of
// being numbered by hand, and everything of Grid's -- spacing here -- still
// applies, because a Rows is a Grid.
[[maybe_unused]] void even_stacks() {
    Rows rows{
        rowSpacing = 8.0,
        TextBlock{L"top third"},
        Button{L"middle third"},
        TextBlock{L"bottom third"},
    };
    Columns columns{
        Button{L"left half"},
        Button{L"right half"},
    };
}

// Rows and Columns are read from a sender as Card is (controls.surface.cpp):
// through the protected constructor each of them declares.
[[maybe_unused]] void hand_written_panels_as_senders() {
    Rows{}.add_onActualThemeChanged([](Rows const& self, Object const&) { self.rowSpacing(4); });
    Columns{}.add_onActualThemeChanged(
        [](Columns const& self, Object const&) { self.columnSpacing(4); });
}

// SplitPanel -- written by hand: its orientation and pane placement take the tags
// of StackPanel and SplitView.
[[maybe_unused]] void SplitPanel_vertical_trailing(::wxl::UIElement const& top,
                                                   ::wxl::UIElement const& bottom) {
    using namespace ::wxl::dsl;
    ::wxl::SplitPanel const split{orientation.vertical, panePlacement.right, openPaneLength = 200.0,
                                  content = top, pane = bottom};
    (void)split;
}

// A layout written as two functions, in a LayoutPanel.
[[maybe_unused]] void custom_layout() {
    auto const custom = CustomLayout{
        [](Collection<UIElement> const& children, Size available) { return available; },
        [](Collection<UIElement> const& children, Size final) { return final; },
    };
    LayoutPanel{layout = custom};
}

// AvailableSizeLayout -- written by hand, so its schema lines are too. The two properties are reported, never taken:
// BindInput is the one form they accept.
[[maybe_unused]] void AvailableSizeLayout_availableWidth_bound(
    ::wxl::AvailableSizeLayout const& object, ::wxl::core::observable<double>& field) {
    ::wxl::impl::apply_argument(
        object, ::wxl::dsl::schema::AvailableSizeLayout::availableWidth = ::wxl::BindInput{field});
    ::wxl::impl::apply_argument(
        object, ::wxl::dsl::schema::AvailableSizeLayout::availableHeight = ::wxl::BindInput{field});
}
[[maybe_unused]] void AvailableSizeLayout_in_braces(::wxl::core::observable<double>& width,
                                                    ::wxl::core::observable<double>& height) {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    LayoutPanel{layout = AvailableSizeLayout{availableWidth = BindInput{width},
                                             availableHeight = BindInput{height}},
                TextBlock{u"page"}};
}

}  // namespace
