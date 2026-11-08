// Intro of the example: what SetMathML and GetMathML do and how math mode is
// turned on. The last paragraph is a <div>, so it carries no paragraph margin;
// the theme gives every <p> four pixels below, as the original Paragraphs had.
auto description = HtmlBlock {};
description.theme(MarkupTheme{.paragraphMargin = Thickness {0, 0, 0, 4}});
description.registerStyle(u"semibold", {.text = {.weight = 600.0}});
description.html(
    u"<p>The <span style=\"semibold\">SetMathML</span> method takes a <a href=\"https://www.w3.org/Math/\">MathML</a> "
    u"string and displays the equation in the <span style=\"semibold\">RichEditBox</span>. It replaces any existing "
    u"equation with the new one.</p>"
    u"<p>The <span style=\"semibold\">GetMathML</span> method retrieves the MathML string of the equation from the "
    u"<span style=\"semibold\">RichEditBox</span>. However, it only works if the equation is in a single line. If the text "
    u"spans multiple lines, it returns an empty string, but the equation will still be rendered correctly.</p>"
    u"<p>Setting the math mode in the <span style=\"semibold\">RichEditBox</span> is necessary to use these methods. it can "
    u"be enabled using <code>SetMathMode(RichEditMathMode.MathOnly)</code>.</p>"
    u"<div><span style=\"semibold\">SetMathML</span> and <span style=\"semibold\">GetMathML</span> can be used to restore "
    u"and save equations.</div>");

// What the editor holds is shown under it as MathML, as often as it changes.
auto mathml = RsdnBlock {isTextSelectionEnabled = true};

auto editor = RichEditBox {
    fontSize = 16,
    height = 80,
    width = 724,
    hAlign.left,
    onTextChanged += [mathml](RichEditBox const& self) {
        // Only a formula in one line has MathML to give: more lines give an empty string.
        auto const text = self.document().getMathML();
        mathml.rsdn(L"[code=xml]" +
                    (text.empty() ? std::wstring{L"<!-- No MathML content -->"}
                                  : gallery::indentXml(gallery::wide(text))) +
                    L"[/code]");
    },
};
editor.document().setMathMode(RichEditMathMode::MathOnly);

auto const formula = std::u16string {
    u"<mml:math xmlns:mml=\"http://www.w3.org/1998/Math/MathML\" display=\"block\">\r\n"
    u"  <mml:mi mathcolor=\"#000000\">x</mml:mi>\r\n"
    u"  <mml:mo mathcolor=\"#000000\">\u2208</mml:mo>\r\n"
    u"  <mml:mi mathcolor=\"#000000\">P</mml:mi>\r\n"
    u"  <mml:mfenced>\r\n"
    u"    <mml:mrow>\r\n"
    u"      <mml:mi mathcolor=\"#000000\">A</mml:mi>\r\n"
    u"    </mml:mrow>\r\n"
    u"  </mml:mfenced>\r\n"
    u"  <mml:mo mathcolor=\"#000000\">\u2194</mml:mo>\r\n"
    u"  <mml:mi mathcolor=\"#000000\">x</mml:mi>\r\n"
    u"  <mml:mo mathcolor=\"#000000\">\u2286</mml:mo>\r\n"
    u"  <mml:mi mathcolor=\"#000000\">A</mml:mi>\r\n"
    u"</mml:math>"};

auto setFormula = Button {
    u"Set sample formula",
    styles.Button.Accent,
    onClick += [editor, formula] {
        // The formula's own colour has to be the text colour of the theme.
        auto text = formula;
        if (editor.actualTheme() == ElementTheme::Dark) {
            for (std::size_t at; (at = text.find(u"#000000")) != std::u16string::npos;) {
                text.replace(at, 7, u"#FFFFFF");
            }
        }
        editor.document().setMathML(text);
    },
};