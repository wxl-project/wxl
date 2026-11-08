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
    BbBlock message{
        L"Привет, [b]мир[/b]! [url=https://example.invalid]пример[/url]",
        isTextSelectionEnabled = true,
    };
    message.theme(MarkupTheme{});
    message.baseDirectory(LR"(C:\docs)");
    message.onLink([](zstring_view) {});
    message.onError([](MarkupError const&) {});
    message.append(L"[quote=Вася]дописано[/quote]");
    message.bb(L"[list][*]заново[/list]");
}

}  // namespace
