#pragma once

// The elements of the window, built from the model: ordinary ones bound to its scalar fields and
// to the count of its items, and a list bound to the items themselves. The library stands on the
// profile of the solution, whose project gives it the wrappers and the component dsl with the
// bindings.

#include "Bind.h"
#include "ui.h"

#include "model.h"

namespace views {

// The title and the number of items.
wxl::StackPanel header(model::Model& model);

// The items, one row each; a change of the list reaches the rows as that change.
wxl::ListView itemList(model::Model& model);

// The box the next item is typed into, and the buttons that add and remove items.
wxl::StackPanel editor(model::Model& model);

}
