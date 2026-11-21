#pragma once

// The model of the shopping list, on wxl.core alone: a title and the text being typed, scalar
// observables, and the items, a vector observable. The library views builds the elements of the
// window from it and binds them to these fields.

import std;
import wxl.core;

namespace model {

struct Model : wxl::core::sta_refcounted {
    wxl::core::observable<wxl::core::u16_text> title {wxl::core::u16_text {u"Shopping list"}};
    wxl::core::observable<wxl::core::u16_text> draft;
    wxl::core::observable_list<wxl::core::u16_text> items;

    Model();

    // Moves the draft into the items, unless it is empty.
    void add();
    void removeFirst();
};

}
