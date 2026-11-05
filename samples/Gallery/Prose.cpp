// Проза страницы Gallery: текст, который страница показывает между живыми примерами. Он лежит в .html рядом со
// сниппетами страницы (Snippets/<Страница>/<Раздел>.html) и строится тем же HtmlBlock, что Presenter ставит введением
// к примеру, с одним обработчиком ссылок: item:<id> — переход на страницу контрола, http/https — запуск адреса,
// любая иная схема — ошибка (не маскируется).

#include "Pages.h"
#include "Shell.h"

#include <cassert>

#include <wxl/Windows.System.h>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

// Запуски адресов по ссылкам http/https: задача живёт, пока адрес не открыт, а кончившаяся выметается при следующем
// запуске — как сохранения состояния в Shell.
std::vector<async::task<bool>> launches;

void launch(zstring_view uri) {
    std::erase_if(launches, [](async::task<bool> const& each) { return each.done(); });
    launches.push_back(Launcher::launchUriAsync(uri));
}

}  // namespace

namespace gallery {

// Текст сниппета: из UTF-8 в UTF-16, без BOM, без CR (git на Windows мог отдать файл с CRLF) и без пустых строк по
// краям.
std::wstring snippetText(Snippet bytes) {
    auto const checked = core::unicode::checked(bytes);
    if (!checked) {
        return {};
    }
    std::wstring wide = checked->to_utf16();
    std::erase(wide, L'\r');
    std::wstring_view body{wide};
    if (body.starts_with(L'\ufeff')) {
        body.remove_prefix(1);
    }
    while (!body.empty() && body.front() == L'\n') {
        body.remove_prefix(1);
    }
    while (!body.empty() && body.back() == L'\n') {
        body.remove_suffix(1);
    }
    return std::wstring{body};
}

FrameworkElement prose(Snippet html, Thickness margin) {
    // Прозу страница пишет абзацами и переносами, как RichTextBlock оригинала: между <p> отступа нет, пустая
    // строка — это <br>.
    MarkupTheme theme;
    theme.paragraphMargin = Thickness {0};

    auto block = HtmlBlock {
        Margin {margin},
        isTextSelectionEnabled = true,
    };
    block.theme(theme);
    block.html(snippetText(html));
    block.onLink([](zstring_view target) {
        auto const view = target.view();
        if (view.starts_with(u"item:")) {
            navigate({Place::Item, wide(view.substr(5))});
            return;
        }
        if (view.starts_with(u"http://") || view.starts_with(u"https://")) {
            launch(target);
            return;
        }
        // Схемы, которой в prose не бывает, быть не должно: это ошибка в .html страницы, и её не глушат.
        assert(false && "prose: link scheme is not item:, http:// or https://");
    });
    return block;
}

}  // namespace gallery
