auto editor = RichEditBox {fontSize = 16, height = 80, width = 724, hAlign.left};
editor.document().setMathMode(RichEditMathMode::MathOnly);

auto description = HtmlBlock {};
description.theme(MarkupTheme{.paragraphMargin = Thickness {0, 0, 0, 4}});
description.html(
    u"<p>Math mode enables users to have input automatically recognized and converted to math expressions while being received.</p>"
    u"<p>It uses <a href=\"https://www.unicode.org/notes/tn28/\">Unicode Nearly Plain-Text Encoding of Mathematics</a>, which allows "
    u"mathematical notation to be represented in a linear format and automatically converted into proper math equations.</p>"
    u"<p>For example, \"4^2\" is converted to \"4\u00b2\", and \"\\pi\" is converted to \"\u03c0\".</p>"
    u"<div>Enabling math mode in a RichEditBox automatically switches the input font to Cambria Math. Additionally, toggling math mode "
    u"clears any existing content and undo stack.</div>");