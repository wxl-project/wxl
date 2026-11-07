// Сниппет, прочитанный с диска при первом обращении (LazyLoadedText), и
// перегрузка gallery::snippet для него: байты приходят не из массива в exe, а
// из файла рядом с ним. Пилот — RadialGradientBrushPage.

#include "Pages.h"

import std;
import wxl.core;

using namespace wxl;

namespace gallery {

LazyLoadedText::LazyLoadedText(std::string_view relative) : relative_{relative} {}

Snippet LazyLoadedText::bytes() const {
    if (!loaded_) {
        // Папка exe плюс относительный путь: туда wxl_target_assets копирует
        // Snippets. read_all читает байты, не текст; UTF-8 проверяет
        // snippetText, а не это чтение.
        auto const path = core::environment::application_folder() / relative_;
        auto const bytes = core::file::read_all(path.c_str());

        if (!bytes) {
            // Одна фраза на всё чтение, той же формы, что у xml/json в wxl.core:
            // полный путь, по которому читали, слова системы о коде и сам код.
            throw std::runtime_error(std::format(
                "cannot read {}: {} (system error code = {})",
                core::unicode::to_utf8(path).chars(),
                core::environment::error_explanation(
                    static_cast<std::uint32_t>(bytes.error().value())).chars(),
                bytes.error().value()));
        }

        bytes_ = std::move(*bytes);
        loaded_ = true;
    }

    // Те же байты, что лежат в строке: char -> char8_t — переозначение тех же
    // байтов, не копия.
    return Snippet{reinterpret_cast<char8_t const*>(bytes_.data()), bytes_.size()};
}

Snippet snippet(LazyLoadedText const& lazy) {
    return lazy.bytes();
}

}  // namespace gallery
