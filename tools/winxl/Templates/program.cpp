// The program of @NAME@: wxl calls wxl_launched once the Windows App Runtime is up, and it opens
// the first window.

#include "launch.h"
#include "ui.h"

using namespace wxl;
using namespace wxl::dsl;

Teardown wxl_launched() {
    auto window = Window {
        title = u"@NAME@",
        TextBlock {text = u"@NAME@ on wxl"},
    };
    window.activate();
    return {};
}
