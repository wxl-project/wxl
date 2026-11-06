#pragma once

// What stands behind the shapes and geometries written in XAML's own mini-languages: the vertices
// of a Polyline or a Polygon, and the path language a Geometry is built from. The framework's own
// converter reads the text in both.
//
// Private: the shape arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>

#include "../hstring_param.h"

namespace wxl::impl {

// `points = u"10,100 60,40"` on a Polyline or a Polygon. Points is a collection of Point, and wxl
// has no class for a collection of values; the vertices are written the way XAML writes them, and
// the framework's own converter makes the PointCollection from the text.

void set_points(winrt::Microsoft::UI::Xaml::Shapes::Polyline const& shape,
                hstring_param const& text);
void set_points(winrt::Microsoft::UI::Xaml::Shapes::Polygon const& shape,
                hstring_param const& text);

/// `data = u"F1 M 20,20 L 24,10"` on a PathIcon or a Path. A Geometry is abstract, and the ones a
/// path is drawn with are built by the framework's own converter from XAML's mini-language and by
/// nothing simpler; the wrapper of Geometry takes a string and has this make the object.
winrt::Microsoft::UI::Xaml::Media::Geometry geometry_from_text(hstring_param const& text);

}  // namespace wxl::impl
