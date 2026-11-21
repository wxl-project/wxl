// An application on the library of a profile over extended, built from an
// installed wxl: a BbBlock, an RsdnBlock and a CodeBlock filled with text, so
// that the link needs the archives of wxl.bb, wxl.rsdn and wxl.highlight.

#include "BbBlock.h"
#include "CodeBlock.h"
#include "RsdnBlock.h"
#include "launch.h"
#include "ui.h"

using namespace wxl;

Teardown wxl_launched() {
    using namespace wxl::dsl;
    auto bb = BbBlock {};
    bb.bb(u"[b]wxl.bb[/b] from an installed prefix");
    auto rsdn = RsdnBlock {};
    rsdn.rsdn(u"[c#]var text = \"wxl.rsdn\";[/c#]");
    auto code = CodeBlock {};
    code.code(u"cpp", u"int main() { return 0; }");

    auto window = Window {
        title = u"install-extended",
        StackPanel {bb, rsdn, code},
    };
    window.activate();
    return {};
}
