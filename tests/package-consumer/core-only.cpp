// wxl.core from the package: the modules of wxl.core and fmt built from the
// interfaces the package carries, and the archives of both linked. format()
// calls into fmt; the line is the version of the package.

import std;
import wxl.core;

int main() {
    const auto text = wxl::core::format(u"{} + {} = {}", 2, 2, 4);
    std::println("wxl {}: {} code units", WXL_VERSION, text.size());
}
