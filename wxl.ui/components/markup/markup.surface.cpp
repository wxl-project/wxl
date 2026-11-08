// Compile-time check of the declarative surface of the markup blocks.
// Built into the surface test of every profile that selects the component,
// beside test/dsl_surface.cpp; nothing here runs.

#include "FormattedBlock.h"
#include "HtmlBlock.h"
#include "ui.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// FormattedBlock: the declarative form only creates -- properties of the
// RichTextBlock underneath -- while content arrives through the procedural
// appenders. Both halves have to compile without a winrt header in sight.
[[maybe_unused]] void formatted_block() {
    FormattedBlock block{
        fontSize = 14.0,
        isTextSelectionEnabled = true,
    };

    block.onLink([](zstring_view) {});
    block.onError([](MarkupError const& error) {
        (void)error.kind;
        (void)error.detail;
    });
    block.appendParagraph();
    block.appendParagraph({.margin = Thickness{0, 0, 0, 8}, .alignment = TextAlignment::Center});
    block.pushStyle({.weight = fontWeightBold});
    block.appendText(L"WinUI");
    block.popStyle();
    block.appendText(L" in C++ Without XAML!");
    block.appendLineBreak();
    block.appendLink(L"wxl", L"https://example.invalid/wxl");
    block.pushLink(L"note:1");
    block.appendText(L"сноска");
    block.popLink();
    block.appendScript(L"2", 0.62, 0.25);
    block.appendImage(L"Assets/logo.png", {24, 24}, L"логотип");
    block.appendElement(Button{content = L"Click"});
    block.clear();
}

// HtmlBlock: markup as the positional string, styles registered by name --
// and none of it needing a winrt header, the parser being a module behind
// the library's own wall.
[[maybe_unused]] void html_block() {
    HtmlBlock markup{
        L"Привет, <b>мир</b>! <a href='faq'>Подробности</a>.",
        isTextSelectionEnabled = true,
        lineHeight = 22.0,
        lineStackingStrategy = LineStackingStrategy::BlockLineHeight,
    };

    markup.registerStyle(L"quote",
                         {.text = {.italic = true}, .block = {.margin = Thickness{24, 4, 0, 4}}});
    markup.registerStyles({
        {L"warn", {.text = {.color = rgb(192, 57, 43), .weight = fontWeightBold}}},
        {L"note", {.text = {.color = rgb(96, 96, 96)}}},
        {L"semibold", {.text = {.weight = 600.0}}},
    });
    markup.onLink([](zstring_view) {});
    markup.onError([](MarkupError const&) {});
    markup.theme(MarkupTheme{.quoteColor = {rgb(128, 128, 128)},
                             .preRadius = 6,
                             .preElevation = 24,
                             .preBackground = rgb(242, 242, 242),
                             .listIndent = 32});
    markup.baseDirectory(LR"(C:\docs)");
    markup.append(L"<p style=\"note\">дописано <i>куском</i></p>");
    markup.html(L"<h1>Заново</h1>");
}

}  // namespace
