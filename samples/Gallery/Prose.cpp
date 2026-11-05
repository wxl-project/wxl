// Проза страницы Gallery: текст, который страница показывает между живыми примерами. Он лежит в .html рядом со
// сниппетами страницы (Snippets/<Страница>/<Раздел>.html) и строится тем же HtmlBlock, что Presenter ставит введением
// к примеру, с обработчиком ссылок (item:<id> — переход на страницу контрола, http/https — запуск адреса, иная
// схема — ошибка) и onError: восстановление парсера и заблокированная ссылка тоже не маскируются.

#include "Pages.h"
#include "Shell.h"

#include <stdexcept>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace gallery {

// Текст сниппета: из UTF-8 в UTF-16, без BOM, без CR (git на Windows мог отдать файл с CRLF) и без пустых строк по
// краям.
std::wstring snippetText(Snippet bytes) {
    auto const checked = core::unicode::checked(bytes);
    if (!checked) {
        throw std::invalid_argument("snippetText: the snippet is not valid UTF-8");
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
            launchUri(target);
            return;
        }
        // Схемы, которой в prose не бывает, быть не должно: это ошибка в .html страницы, и её не глушат — останавливает
        // и в Release.
        throw std::invalid_argument(
            "prose: link scheme is not item:, http:// or https://: " +
            std::string{core::unicode::assume_valid(view).to_utf8().chars()});
    });
    // Восстановление парсера — ошибка в .html страницы, как и заблокированная ссылка: тоже не глушится.
    block.onError([](MarkupError const& error) {
        throw std::invalid_argument(
            "prose: markup error: " +
            std::string{core::unicode::assume_valid(error.detail.view()).to_utf8().chars()});
    });
    return block;
}

}  // namespace gallery
