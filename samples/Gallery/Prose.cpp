// Проза страницы Gallery: текст, который страница показывает между живыми примерами. Он лежит в .html рядом со
// сниппетами страницы (Snippets/<Страница>/<Раздел>.html) и строится тем же HtmlBlock, что Presenter ставит введением
// к примеру, с обработчиком ссылок (item:<id> — переход на страницу контрола, http/https — запуск адреса, иная
// схема — ошибка) и onError: восстановление парсера не маскируется, а останавливает. onLink установлен всегда, поэтому
// BlockedLink — клик, погашенный политикой по умолчанию, когда обработчика ссылок нет, — здесь невозможен.

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

// Имя рода ошибки разметки для сообщения об исключении — как зовётся сам элемент перечисления.
char const* errorName(MarkupErrorKind kind) {
    switch (kind) {
        case MarkupErrorKind::UnknownTag: return "UnknownTag";
        case MarkupErrorKind::UnpairedClose: return "UnpairedClose";
        case MarkupErrorKind::MisnestedTags: return "MisnestedTags";
        case MarkupErrorKind::BadEntity: return "BadEntity";
        case MarkupErrorKind::MisplacedTag: return "MisplacedTag";
        case MarkupErrorKind::UnknownStyle: return "UnknownStyle";
        case MarkupErrorKind::BadColor: return "BadColor";
        case MarkupErrorKind::BadFontSize: return "BadFontSize";
        case MarkupErrorKind::BadDimension: return "BadDimension";
        case MarkupErrorKind::MissingImage: return "MissingImage";
        case MarkupErrorKind::RemoteImage: return "RemoteImage";
        case MarkupErrorKind::ImageFailed: return "ImageFailed";
        case MarkupErrorKind::BlockedLink: return "BlockedLink";
    }
    return "?";
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
    // Полужирный вес 600 — то, чего не умеет ни один тег разметки (<b>/<strong> — это 700), поэтому он приходит
    // именованным стилем (<span style="semibold">…</span>) и регистрируется здесь, до html(): реестр читается при
    // разборе, и зарегистрированный после стиль уже построенный текст не перекрасит.
    block.registerStyle(u"semibold", {.text = {.weight = 600.0}});
    // Обработчики — до html(): парсер сообщает о восстановлениях по ходу html() в слив onError, и обработчик,
    // поставленный после, их уже не увидит. onLink — тот же порядок ради одного места, где решено, что тут
    // происходит с кликом.
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
    // Восстановление парсера и сборщика — ошибка в .html страницы: тоже не глушится, а останавливает (и в Release). Род
    // ошибки — в сообщении, рядом с деталью. BlockedLink сюда не придёт: onLink установлен всегда, и политика по
    // умолчанию, которая одна и рождает BlockedLink, не вступает.
    block.onError([](MarkupError const& error) {
        throw std::invalid_argument(
            "prose: markup error (" + std::string{errorName(error.kind)} + "): " +
            std::string{core::unicode::assume_valid(error.detail.view()).to_utf8().chars()});
    });
    block.html(snippetText(html));
    return block;
}

}  // namespace gallery
