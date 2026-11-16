// Compile-time check of the declarative surface of the code block.
// Built into the surface test of every profile that selects the component;
// nothing here runs.

#include "CodeBlock.h"
#include "ui.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// CodeBlock: the language by name, the source as plain text, the code part of
// MarkupTheme for the look -- and none of it needing a winrt header or the
// highlighting module, which stays behind the library's own wall.
[[maybe_unused]] void code_block() {
    CodeBlock code{
        isTextSelectionEnabled = true,
        Padding{16, 12, 56, 12},
    };
    code.theme(MarkupTheme{.monospace = u"Cascadia Mono",
                           .keywordColor = rgb(0, 0, 255),
                           .stringColor = rgb(139, 0, 0),
                           .commentColor = rgb(0, 128, 0)});
    code.code(u"cpp", u"int main() { return 0; }  // done");
    code.code(L"xml", std::wstring{L"<!-- comment -->\n<a href=\"x\"/>"});
    code.code(u"no-such-language", u"shown plain");
}

}  // namespace
