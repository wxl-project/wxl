// The intermediateValue pair, the winrt side: wired so that nothing holds the
// model and nothing outlives what it points at.
//
// The projection comes first, and with it the standard library it pulls in:
// the wxl headers below carry the wxl.core import, and a standard header after
// that import is one MSVC has already seen through the std module.
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>

#include "Object.impl.h"
#include <wxl/Members.h>
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Controls.impl.h>
#include "intermediate_value_pair.h"

namespace wxl::impl {
namespace {

// What the pair leaves on the box: the Loaded handler that finds the inner
// text box, and the TextChanged handler it puts there. Held by the field's
// watch alone, so both come off when the field goes or unbind() runs -- while
// the handlers still name the field by bare address.
struct intermediate_revoker : core::sta_refcounted {
    NumberBox box;
    EventToken loaded;
    winrt::Microsoft::UI::Xaml::Controls::TextBox input{nullptr};
    winrt::event_token changed;

    explicit intermediate_revoker(NumberBox const& b) : box(b) {}

    ~intermediate_revoker() {
        if (input) input.TextChanged(changed);
        if (loaded) event_adder<EventKey::Loaded>::remove(box, loaded);
    }
};

}  // namespace

// NumberBox commits to Value on Enter, a spin or the focus leaving, and says
// nothing in between; the number as typed is read off the TextBox inside its
// template instead, on every change of its text. The text is parsed by the
// box's own NumberFormatter -- the parser the box will use itself when it
// commits, so the field sees what Value is about to become, in the box's own
// locale. Text that is not a number yet ("-", "1e") parses to nothing, and
// the field is empty, as for an empty box.
//
// The template is applied once the box is in the tree, so the inner box is
// found on Loaded, and once: a box unloaded and loaded again keeps its
// template. The handler holds the parser, not the box -- a handler on the
// inner box holding the outer would be a cycle -- and the field by address,
// which the guard makes safe. The direction is input: the pair's own is, and
// bind_property refuses every other form on it before it gets here.
void apply_bind_intermediate_value(NumberBox const& control,
                                   core::observable<core::nullable<double>>& model,
                                   bind_direction) {
    using Args = typename event_adder<EventKey::Loaded>::template args_t<NumberBox>;
    using winrt::Windows::Globalization::NumberFormatting::INumberParser;
    namespace xaml = winrt::Microsoft::UI::Xaml;

    core::intrusive_ptr<intermediate_revoker> revoker{new intermediate_revoker{control},
                                                      /*add_ref=*/false};
    intermediate_revoker* const state = revoker.get();

    state->loaded = event_adder<EventKey::Loaded>::add(
        control, [&model, state](NumberBox const& sender, Args&) {
            if (state->input) return;

            // GetTemplateChild is protected, and the projection puts it on the
            // protected interface rather than on the class.
            xaml::Controls::NumberBox const& box = *Object::Impl::get_typed<NumberBox>(sender);
            auto const input = box.as<xaml::Controls::IControlProtected>()
                                   .GetTemplateChild(L"InputBox")
                                   .try_as<xaml::Controls::TextBox>();
            if (!input) return;
            INumberParser const parser = box.NumberFormatter().as<INumberParser>();

            state->input = input;
            state->changed = input.TextChanged(
                [&model, parser](winrt::Windows::Foundation::IInspectable const& sender,
                                 xaml::Controls::TextChangedEventArgs const&) {
                    auto const number =
                        parser.ParseDouble(sender.as<xaml::Controls::TextBox>().Text());
                    model.set(number ? core::nullable<double>{number.Value()}
                                     : core::nullable<double>{});
                });
        });

    model.watch_for_binding([revoker = std::move(revoker)](core::nullable<double> const&) noexcept {});
}

}  // namespace wxl::impl
