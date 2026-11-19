// Страница HeaderedContentControl Community Toolkit: контрол wxl (по образцу SettingsCard, decisions/0465).

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/HeaderedContentControl/HeaderedContentControlBasic.html"};
gallery::LazyLoadedText basicCode{"Snippets/HeaderedContentControl/HeaderedContentControlBasic.h"};

FrameworkElement basicExample() {
#include "Snippets/HeaderedContentControl/HeaderedContentControlBasic.h"

    return gallery::controlExample({.header = gallery::snippet(basicHeader), .example = example, .code = gallery::snippet(basicCode)});
}

}  // namespace

static wxl::FrameworkElement headeredContentControlPage() {
    return StackPanel {basicExample()};
}

REGISTER_GALLERY_PAGE(L"HeaderedContentControl", headeredContentControlPage);
