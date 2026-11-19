#pragma once

// wxl::CodeBlock -- source code with syntax highlighting, filled straight from
// the pieces wxl.highlight splits it into: no markup is written and none is
// parsed. The language is a name highlight::find_language() knows (`cpp`,
// `c#`, `xml`); a name it does not know shows the text plain. Either way the
// text is monospace and keeps its spaces and line breaks.
//
// The look is the code part of MarkupTheme -- the monospace family and the
// keyword, string and comment colours -- with the light and dark palette the
// markup blocks fall back to when a colour is left empty. The rest of the
// theme is not read.

#include <string_view>

#include "MarkupBlock.h"

namespace wxl {

class CodeBlock : public FormattedBlock
{
    using base_t = FormattedBlock;

public:
    class Impl;

    CodeBlock();

    template <typename... Setters>
        requires impl::setter_pack<CodeBlock, Setters...>
    explicit CodeBlock(Setters&&... setters) : CodeBlock() {
        (impl::apply_argument(*this, std::forward<Setters>(setters)), ...);
    }

    // Replaces the content with `source` highlighted as `language`. The
    // palette is the one of the block's actual theme at this call.
    void code(std::u16string_view language, hstring_param const& source) const;

    // The look of what is filled from now on: the monospace family and the
    // three code colours of `value`; content already shown keeps the look it
    // was filled with.
    void theme(MarkupTheme const& value) const;

protected:
    explicit CodeBlock(Impl* impl) noexcept;

    friend class Object::Impl;
};

}  // namespace wxl
