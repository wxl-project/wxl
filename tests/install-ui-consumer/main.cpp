// An application on the library of a profile built from an installed wxl: one
// window with a line of text, so that the link needs the generated wrappers,
// the sources of the components and wxl::ui.common.

#include "launch.h"
#include "ui.h"

using namespace wxl;
using namespace wxl::dsl;

Teardown wxl_launched() {
    auto window = Window {
        title = u"install-ui",
        TextBlock {text = u"wxl from an installed prefix"},
    };
    window.activate();
    return {};
}
