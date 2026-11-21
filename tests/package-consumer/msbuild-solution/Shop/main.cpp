// The program of the solution: one window over the model of the library model, made of the
// elements the library views builds from it.

#include "launch.h"
#include "ui.h"

#include "model.h"
#include "views.h"

using namespace wxl;
using namespace wxl::dsl;

Teardown wxl_launched() {
    // The bindings and the handlers of the window refer to the model; the Teardown keeps it as
    // long as the window lives.
    auto const shop = core::make_refcounted<model::Model>();
    model::Model& model = *shop;

    auto window = Window {
        title = u"WinXL solution",
        StackPanel {
            Padding {16},
            spacing = 12,
            views::header(model),
            views::editor(model),
            views::itemList(model),
        },
    };
    window.activate();
    return [shop](TeardownReason) {};
}
