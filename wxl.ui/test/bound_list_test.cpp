// A list bound to a ListView on a profile that selects no component of list controls: extended.
// The binding is the generated attach of ListViewBase (BindingPairs.h) over the machinery of
// wxl.ui.common, so the program links with no component of the profile behind it -- the link is
// the check that matters. Run, it shows the list and ends once the control has asked for the
// element of the first item; its exit code says whether it did.

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
