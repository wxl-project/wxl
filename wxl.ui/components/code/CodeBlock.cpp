// wxl::CodeBlock: the pieces of wxl.highlight said straight through the
// FormattedBlock primitives -- a run per line of a piece, a style level per
// coloured piece, a LineBreak per line end.

// The projection first (see MarkupBlock.cpp on the order of the headers).
#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include "CodeBlock.h"
#include "FormattedBlock.impl.h"
#include "Object.impl.h"
#include "impl/activator.h"
#include "impl/code_palette.h"

// The import last: MSVC accepts no standard header after it.
import wxl.highlight;

namespace wxl {

namespace impl {

// The wrapper is wxl's own, the object behind it the system's RichTextBlock,
// as for the whole markup family.
template <>
struct runtime_class_name_of<CodeBlock> {
    static constexpr wchar_t value[] = L"Microsoft.UI.Xaml.Controls.RichTextBlock";
};

}  // namespace impl

class CodeBlock::Impl : public base_t::Impl
{
public:
    using base_t::Impl::Impl;

    // The look code() fills with.
    MarkupTheme theme_;
};

namespace {

// The text of the source as runs and line breaks. A run takes a terminated
// string and a piece is a slice of the source, so each line of a piece is
// copied into one scratch buffer, reused by every run of the fill. \r\n, a
// lone \r and \n each end one line, and a \r\n split between two pieces is
// still one.
class line_writer
{
public:
    explicit line_writer(FormattedBlock const& block) noexcept : block_(block) {}

    void write(std::u16string_view text) {
        std::size_t start = 0;
        for (std::size_t at = 0; at < text.size(); ++at) {
            const char16_t c = text[at];
            if (c != u'\n' && c != u'\r') continue;
            run(text.substr(start, at - start));
            const bool after_cr = at == 0 ? after_cr_ : text[at - 1] == u'\r';
            if (c == u'\r' || !after_cr) block_.appendLineBreak();
            start = at + 1;
        }
        if (!text.empty()) after_cr_ = text.back() == u'\r';
        run(text.substr(start));
    }

private:
    void run(std::u16string_view line) {
        if (line.empty()) return;
        scratch_.assign(line);
        block_.appendText(scratch_);
    }

    FormattedBlock const& block_;
    core::sta_u16string scratch_;
    bool after_cr_ = false;
};

}  // namespace

CodeBlock::CodeBlock(Impl* impl) noexcept : base_t(impl) {}

CodeBlock::CodeBlock() : base_t(new Impl{impl::activation<impl::activator<CodeBlock>>{}}) {}

void CodeBlock::code(zstring_view language, hstring_param const& source) const {
    MarkupTheme const& theme = static_cast<Impl*>(impl())->theme_;
    clear();
    // On the runs rather than on the block, as the markup blocks wear it on
    // <pre>: a line break keeps the height of the block's own font.
    pushStyle({.fontFamily = theme.monospace});

    line_writer lines(*this);
    const std::u16string_view text = source.text().view();
    if (const highlight::language* found = highlight::find_language(language.view())) {
        // The colours once per fill; the actual theme is asked only when the
        // theme leaves a colour to the palette.
        TextStyle styles[3];
        core::nullable<bool> dark;
        for (std::size_t i = 0; i < 3; ++i) {
            const auto kind = static_cast<impl::code_kind>(i);
            const core::nullable<Color>& themed = impl::themed_code_color(theme, kind);
            if (!themed && !dark) dark = actualTheme() == ElementTheme::Dark;
            styles[i].color = themed ? *themed : impl::palette_code_color(kind, *dark);
        }

        highlight::scanner scan(*found, text);
        while (const std::optional<highlight::piece> next = scan.next()) {
            const TextStyle* style = nullptr;
            switch (next->kind) {
                case highlight::kind_t::keyword:
                    style = &styles[static_cast<std::size_t>(impl::code_kind::keyword)];
                    break;
                case highlight::kind_t::string:
                    style = &styles[static_cast<std::size_t>(impl::code_kind::string)];
                    break;
                case highlight::kind_t::comment:
                    style = &styles[static_cast<std::size_t>(impl::code_kind::comment)];
                    break;
                case highlight::kind_t::plain:
                    break;
            }
            if (style) pushStyle(*style);
            lines.write(next->text);
            if (style) popStyle();
        }
    } else {
        lines.write(text);
    }

    popStyle();
}

void CodeBlock::theme(MarkupTheme const& value) const {
    static_cast<Impl*>(impl())->theme_ = value;
}

}  // namespace wxl
