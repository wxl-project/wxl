#include "@NAME@.h"

using namespace wxl;

namespace @NAME@ {

TextBlock greeting() {
    using namespace wxl::dsl;
    return TextBlock {text = u"Hello from @NAME@"};
}

}
