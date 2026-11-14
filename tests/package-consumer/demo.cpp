// An application on the library of a profile made from the package: one
// window with a line of text and a button, so that the link needs the
// generated wrappers, the sources of the components and wxl::ui.common.

#include "launch.h"
#include "ui.h"

using namespace wxl;
using namespace wxl::dsl;

Teardown wxl_launched() {
    auto window = Window {
        title = u"WinXL package consumer",
        StackPanel {
            Padding {16},
            spacing = 8,
            TextBlock {text = u"wxl from the NuGet package WinXL"},
            Button {content = u"Close"},
        },
    };
    window.activate();
    return {};
}
