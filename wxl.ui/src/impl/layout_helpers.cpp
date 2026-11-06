// The projection comes first, and with it every standard header it needs:
// wxl's own headers carry the wxl.core import, and a standard header included
// after that import is one the compiler has already seen through the std
// module.
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.Collections.h>

#include "layout_helpers.h"

#include "conversions.h"

#include "../Object.impl.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>
#include <wxl/Structs.impl.h>

// Imports last, after every plain header.
import wxl.core;

namespace wxl::impl {

namespace {

// The one building of a grid's definitions, whether the lengths were parsed from a text or
// computed: the old ones go, and one definition per length is appended in order.
void build_rows(winrt::Microsoft::UI::Xaml::Controls::Grid const& grid,
                std::span<GridLength const> lengths) {
    auto const rows = grid.RowDefinitions();
    rows.Clear();
    for (GridLength const length : lengths) {
        winrt::Microsoft::UI::Xaml::Controls::RowDefinition row;
        row.Height(to_winrt(length));
        rows.Append(row);
    }
}

void build_columns(winrt::Microsoft::UI::Xaml::Controls::Grid const& grid,
                   std::span<GridLength const> lengths) {
    auto const columns = grid.ColumnDefinitions();
    columns.Clear();
    for (GridLength const length : lengths) {
        winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition column;
        column.Width(to_winrt(length));
        columns.Append(column);
    }
}

}  // namespace

void set_row_definitions(winrt::Microsoft::UI::Xaml::Controls::Grid const& grid,
                         RowLengths const& rows) {
    build_rows(grid, rows.lengths());
}

void set_column_definitions(winrt::Microsoft::UI::Xaml::Controls::Grid const& grid,
                            ColumnLengths const& columns) {
    build_columns(grid, columns.lengths());
}

void set_row_definitions(Grid const& grid, std::span<GridLength const> lengths) {
    build_rows(*Object::Impl::get_typed<Grid>(grid), lengths);
}

void set_column_definitions(Grid const& grid, std::span<GridLength const> lengths) {
    build_columns(*Object::Impl::get_typed<Grid>(grid), lengths);
}

}  // namespace wxl::impl
