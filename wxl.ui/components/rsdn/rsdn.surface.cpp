// Compile-time check of the declarative surface of the RSDN block.
// Built into the surface test of every profile that selects the component,
// beside test/dsl_surface.cpp; nothing here runs.

#include "RsdnBlock.h"
#include "ui.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// The RSDN sibling of BbBlock, over the same MarkupBlock.
[[maybe_unused]] void rsdn_block() {
    RsdnBlock post{
        L"AVK>цитата\nответ [c#]var x = 1;[/c#]",
    };
    post.onError([](MarkupError const&) {});
    post.append(L"[q]ещё[/q]");
    post.rsdn(L"[h1]Заново[/h1]");
}

}  // namespace
