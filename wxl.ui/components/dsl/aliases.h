#pragma once

// Short names for the enums the declarative syntax repeats most.
//
// `hAlign.center` instead of `HorizontalAlignment::Center` saves characters
// in the one place the DSL spends them constantly -- and fewer characters is
// the entire justification, which for a syntax whose whole purpose is
// brevity is a sufficient one.
//
// Each shortcut object is an empty type whose members are `static constexpr`,
// reached through the dot because that reads better than `::` here. Nothing
// is stored and nothing is constructed at run time. They are members, so they
// are spelled the way every other member of a wxl type is.

#include <wxl/Microsoft.UI.Xaml.Enums.h>

namespace wxl::dsl {

inline constexpr struct {
    static constexpr HorizontalAlignment left = HorizontalAlignment::Left;
    static constexpr HorizontalAlignment center = HorizontalAlignment::Center;
    static constexpr HorizontalAlignment right = HorizontalAlignment::Right;
    static constexpr HorizontalAlignment stretch = HorizontalAlignment::Stretch;
} hAlign;

inline constexpr struct {
    static constexpr VerticalAlignment top = VerticalAlignment::Top;
    static constexpr VerticalAlignment center = VerticalAlignment::Center;
    static constexpr VerticalAlignment bottom = VerticalAlignment::Bottom;
    static constexpr VerticalAlignment stretch = VerticalAlignment::Stretch;
} vAlign;

// The wrapping of a text block, written without the argument name:
// `TextBlock {textWrap.wholeWords, text}`. TextWrapping::NoWrap is `none`, so
// that the one value which does not wrap does not read as "wrap".
inline constexpr struct {
    static constexpr TextWrapping none = TextWrapping::NoWrap;
    static constexpr TextWrapping wrap = TextWrapping::Wrap;
    static constexpr TextWrapping wholeWords = TextWrapping::WrapWholeWords;
} textWrap;

}  // namespace wxl::dsl
