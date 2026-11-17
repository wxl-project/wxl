// Compile-time check of the declarative surface of the BB-code block.
// Built into the surface test of every profile that selects the component;
// nothing here runs.

#include "BbBlock.h"
#include "ui.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// Братья HtmlBlock над общим MarkupBlock: своя только дверь разметки, всё
// остальное — тема, база картинок, onLink/onError — семейное.
[[maybe_unused]] void bb_block() {
    BbBlock block{
        L"Привет, [b]мир[/b]! [url=https://example.invalid]пример[/url]",
        isTextSelectionEnabled = true,
    };
    block.theme(MarkupTheme{});
    block.baseDirectory(LR"(C:\docs)");
    block.onLink([](zstring_view) {});
    block.onError([](MarkupError const&) {});
    block.append(L"[quote=Вася]дописано[/quote]");
    block.bb(L"[list][*]заново[/list]");
}

}  // namespace
