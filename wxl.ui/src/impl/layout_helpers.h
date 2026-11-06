#pragma once

// What stands behind the layout properties WinUI writes as collections of objects: a Grid's rows
// and columns.
//
// Private: the control arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include "../GridLengths.h"

namespace wxl::impl {

// A Grid's rows and columns written as RowDefinitions<u"2*,*,auto,120">, where WinUI takes one
// RowDefinition object per row; wxl declares the property itself (the profile's
// syntheticMembers). The text was parsed when the code was compiled (GridLengths.h): what arrives
// is GridLength values, and building the rows has nothing left to refuse.

void set_row_definitions(winrt::Microsoft::UI::Xaml::Controls::Grid const& grid,
                         RowLengths const& rows);
void set_column_definitions(winrt::Microsoft::UI::Xaml::Controls::Grid const& grid,
                            ColumnLengths const& columns);

}  // namespace wxl::impl
