// The program of the test install-core: wxl.core imported from the interfaces
// of an installed prefix, and format() calling into fmt, so that the BMIs of
// both modules are built here and the archives of both are linked.

import std;
import wxl.core;

int main() {
    const auto text = wxl::core::format(u"{} + {} = {}", 2, 2, 4);
    std::println("wxl {}: {} code units", WXL_VERSION, text.size());
}
