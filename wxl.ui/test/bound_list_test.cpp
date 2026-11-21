// A list bound to a ListView on a profile that selects no component of list controls: extended.
// The binding is the generated attach of ListViewBase (BindingPairs.h) over the machinery of
// wxl.ui.common. What the build proves: the program links with no component of the profile behind
// it. What a run proves, and only when it ends by itself: the control asked the bound source for
// the element of an item -- the first call of the function ends the application, and the exit
// code is 0. A run that ends any other way -- the window closed -- exits with 1. A control that
// never asks for an element leaves the window open: nothing here ends that run, so it is not a
// test ctest can run unattended.

#include "Application.h"
#include "Bind.h"
#include "launch.h"
#include "ui.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

struct Model : core::sta_refcounted {
    core::observable_list<int> numbers;
    int built = 0;
};

}  // namespace

Teardown wxl_launched() {
    auto model = core::make_refcounted<Model>();
    for (int number = 1; number <= 3; ++number) model->numbers.push_back(number);

    auto window = Window {
        title = u"bound list on extended",
        ListView {
            itemsSource = BindOutput {model->numbers,
                                      [board = model.get()](int const& number) {
                                          if (board->built++ == 0) Application::exit();
                                          return TextBlock {text = number == 1 ? u"one" : u"more"};
                                      }},
        },
    };
    window.activate();
    return [model](TeardownReason) -> std::optional<int> { return model->built > 0 ? 0 : 1; };
}
