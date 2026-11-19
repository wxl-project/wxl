#pragma once

// What stands behind entry and themeEntry, the entries written in place in a ResourceDictionary.
//
// Private: the dictionary arrives as the projection type the wrapper already holds, so this header
// is one only wxl's own sources ever include.

#include <winrt/Microsoft.UI.Xaml.h>

#include "Resource.h"

namespace wxl::impl {

// What stands behind `entry = Resource {...}` on a ResourceDictionary: the
// value, boxed as the framework's own type, under the key. An entry with a key
// that is already there replaces it, as `Insert` on the dictionary does.
void add_resource(winrt::Microsoft::UI::Xaml::ResourceDictionary const& dictionary,
                  Resource const& entry);

// The same under a theme: the entries go in a dictionary of their own that ThemeDictionaries keeps under the theme's name.
void add_theme_dictionary(winrt::Microsoft::UI::Xaml::ResourceDictionary const& dictionary,
                          ThemeDictionary const& theme);

}  // namespace wxl::impl
