#pragma once

// The colours of highlighted code, shared by every block that shows it: the
// markup blocks colour the kw/str/com spans their tokenizers produce, and
// CodeBlock colours the pieces of wxl.highlight directly. A colour the theme
// sets wins; an empty field falls back to the palette of the element's actual
// theme -- the Visual Studio colours, since a blue keyword that reads on white
// does not read on black.

#include "MarkupBlock.h"

namespace wxl::impl {

// The kinds of a piece of code that wear a colour of their own; plain text
// keeps the colour of the block.
enum class code_kind : std::uint8_t
{
    keyword,
    string,
    comment,
};

// By theme, light then dark, and by code_kind.
inline constexpr Color code_palette[2][3] = {
    {rgb(0x00, 0x00, 0xFF), rgb(0x8B, 0x00, 0x00), rgb(0x00, 0x80, 0x00)},
    {rgb(0x56, 0x9C, 0xD6), rgb(0xD6, 0x9D, 0x85), rgb(0x57, 0xA6, 0x4A)},
};

// The colour the theme sets for the kind; empty leaves it to the palette.
constexpr core::nullable<Color> const& themed_code_color(MarkupTheme const& theme,
                                                         code_kind kind) noexcept {
    if (kind == code_kind::keyword) return theme.keywordColor;
    if (kind == code_kind::string) return theme.stringColor;
    return theme.commentColor;
}

// The palette's colour for the kind under a light or a dark theme.
constexpr Color palette_code_color(code_kind kind, bool dark) noexcept {
    return code_palette[dark ? 1 : 0][static_cast<std::size_t>(kind)];
}

}  // namespace wxl::impl
