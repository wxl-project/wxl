// Страница XamlUICommand -- XamlUICommandPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t reusableHeader[] = {
#include "Snippets/XamlUICommand/Reusable.html.embed"
};
constexpr char8_t reusableCode[] = {
#include "Snippets/XamlUICommand/Reusable.h.embed"
};

FrameworkElement reusable() {
#include "Snippets/XamlUICommand/Reusable.h"

    return gallery::controlExample({
        .header = gallery::snippet(reusableHeader),
        .example = example,
        .code = gallery::snippet(reusableCode),
    });
}

}  // namespace

static wxl::FrameworkElement xamlUICommandPage() {
    return StackPanel {reusable()};
}

REGISTER_GALLERY_PAGE(L"XamlUICommand", xamlUICommandPage);
