// Compile-time check that nothing in wxl's headers and module interfaces hides a tag.
//
// An application writes `using namespace wxl::dsl;` at file scope, which makes every
// tag of the syntax (value, text, source, size, ...) a global name. A parameter or a
// local of a wxl template, an inline function or a body of a module interface that
// takes one of those names then hides it, and a build at /W4 reports C4459 at that
// declaration. wxl's own sources never open the namespace that way, so their build does
// not see it; this target does, and raises C4456-C4459 to level 1 and makes them errors.
//
// The surface sources of this target write the syntax; this one opens the namespace
// the way an application does and uses what an application uses beside the syntax --
// fields and lists of a model, bindings, events, functions, tasks, text -- so that the
// bodies behind them are compiled here. Nothing runs. core::format is left out: what it
// instantiates is fmt's own code, whose names are not wxl's to choose.

#include "Bind.h"
#include "Box.h"
#include "event_awaitable.h"
#include "IndexList.h"
#include "ItemTemplate.h"
#include "UiThread.h"
#include "WaitableTimer.h"
#include "ui.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

struct Note : core::sta_refcounted {
    explicit Note(core::u16_view heading) : title(heading) {}

    core::u16_text const title;
    core::observable<core::u16_text> text{core::u16_text{u"Not started"}};
};

struct Model {
    core::observable<int> selected{0};
    core::observable<double> scale{1.0};
    core::observable<bool> ready;
    core::observable<core::u16_text> query, caption;
    core::observable_list<core::u16_text> words;
    core::observable_list<core::intrusive_ptr<Note>> notes;
    core::event<void(int)> counted;
    core::event<void(core::u16_text const&, int) noexcept> renamed;

    Model() {
        caption.follow(selected, scale, [](int at, double by) {
            return at > 0 && by > 1.0 ? core::u16_text{u"enlarged"} : core::u16_text{u"plain"};
        });
        words.follow(query, [](core::u16_text const& typed) {
            core::sta_vector<core::u16_text> found;
            found.push_back(typed);
            return found;
        });
    }
};

[[maybe_unused]] void fields(Model& model) {
    core::event_token watch = model.selected.on_change([](int) noexcept {});
    model.selected.set(model.selected.get() + 1);
    model.selected.remove_change(watch);
    model.ready.set(!model.ready.get());

    core::observable<core::u16_text const>& shown = model.caption;
    (void)shown.get().size();
}

[[maybe_unused]] void lists(Model& model) {
    model.words.push_back(core::u16_text{u"one"});
    model.words.insert(0, core::u16_text{u"zero"});
    model.words.replace(1, core::u16_text{u"uno"});
    core::u16_text const more[] = {core::u16_text{u"two"}, core::u16_text{u"three"}};
    model.words.insert_range(2, more);
    model.words.erase(0);
    core::sta_vector<core::u16_text> fresh;
    fresh.push_back(core::u16_text{u"four"});
    model.words.assign(std::move(fresh));
    core::event_token watch = model.words.on_change([](core::list_change const&) noexcept {});
    model.words.remove_change(watch);
    (void)model.words.count().get();
    (void)model.words.get();
    model.words.clear();

    model.notes.push_back(core::intrusive_ptr<Note>{new Note{u"plan"}, false});
    (void)model.notes[0]->text.get();
}

[[maybe_unused]] void events(Model& model) {
    core::event_token token = model.counted.add([](int) noexcept {});
    model.counted(3);
    model.counted.remove(token);
    model.renamed.add([](core::u16_text const&, int) noexcept {});
    model.renamed(core::u16_text{u"name"}, 1);
    model.renamed.clear();
}

[[maybe_unused]] void functions() {
    core::function<int(int)> const twice = [](int at) { return at * 2; };
    (void)twice(2);
    core::function const ignore = [](wchar_t) {};
    ignore(L'x');
}

[[maybe_unused]] void texts() {
    std::u16string const owner = u"ёж";
    zstring_view const whole = owner;
    zstring_view const tail = whole.substr(1);
    (void)tail.view();
    (void)whole.substr(0, 1);
    (void)whole.wide();

    std::optional<core::u8_view> const checked = core::unicode::checked(std::string_view{"привет"});
    if (checked) {
        core::u16_text const wide = checked->to_utf16();
        (void)core::unicode::code_point_count(*checked);
        core::u16_view const part = wide;
        (void)part.substr(0, 1);
        for (char32_t const point : core::unicode::code_points(*checked)) (void)point;
    }
    core::u16_text built = core::unicode::repaired(std::u16string_view{u"ab"});
    built += core::u16_view{u"cd"};
    built.append(core::u16_view{u"ef"});
    (void)built.to_utf8();
    (void)core::unicode::floor_code_point_boundary(std::u16string_view{u"ab"}, 1);
    (void)core::unicode::find_invalid_utf16(std::wstring_view{L"ab"});
}

[[maybe_unused]] void nullables() {
    core::nullable<Note> const pointer;
    core::nullable<int> const whole = 1;
    core::nullable<unsigned> const count = 2u;
    core::nullable<double> const ratio = 0.5;
    core::nullable<core::duration> const wait;
    core::nullable<async::stop_token> const token;
    (void)(pointer.has_value() && whole.has_value() && count.has_value() && ratio.has_value() &&
           wait.has_value() && token.has_value());
}

[[maybe_unused]] async::task<int> answer() { co_return 42; }

[[maybe_unused]] async::task<> waits(Model& model, Button button, async::stop_token token) {
    int const got = co_await answer();
    (void)got;
    int const count = co_await on_event(model.counted, token);
    (void)count;
    auto const [who, times] = co_await on_event(model.renamed, token);
    (void)who;
    (void)times;
    RoutedEventArgs& args = co_await onClick(button, token);
    (void)args;
    co_await UiThread::onIdle(token);
    WaitableTimer timer{std::chrono::milliseconds{10}, token};
    co_await timer;
}

[[maybe_unused]] void cancellation() {
    async::stop_source source_of_stop;
    async::stop_token const token = source_of_stop.token();
    (void)token.stop_requested();
    source_of_stop.request_stop();
}

[[maybe_unused]] FrameworkElement bound(Model& model) {
    return StackPanel{
        spacing = 8.0,
        TextBox{placeholderText = u"Filter", text = BindInput{model.query}},
        TextBlock{text = BindOutput{model.caption}},
        TextBlock{text = BindOutput{model.words.count(),
                                    [](uint32_t n) {
                                        return n == 0 ? core::u16_text{u"none"}
                                                      : core::u16_text{u"some"};
                                    }}},
        TextBlock{visibility = BindOutput{model.query,
                                          [](core::u16_text const& typed) {
                                              return typed.size() == 0 ? Visibility::Collapsed
                                                                       : Visibility::Visible;
                                          }}},
        NumberBox{value = Bind{model.scale}},
        ToggleSwitch{isOn = Bind{model.ready}},
        ListView{
            height = 200,
            itemsSource =
                BindOutput{model.words,
                           [](core::u16_text const& word) { return TextBlock{text = word}; }},
            selectedIndex = Bind{model.selected},
            onItemClick +=
            [&model](ListView const&, ItemClickEventArgs& clicked) {
                if (core::u16_text const* word = boundItem(model.words, clicked.clickedItem()))
                    (void)word;
            },
        },
        ListView{
            itemsSource = BindOutput{model.notes,
                                     [](core::intrusive_ptr<Note> const& note) {
                                         return StackPanel{
                                             TextBlock{text = note->title},
                                             TextBlock{text = BindOutput{note->text}}};
                                     }},
        },
        ListView{
            itemsSource = IndexList(4),
            itemTemplate =
                [](Object const& item) {
                    return TextBlock{opacity = unboxValue<int64_t>(item) == 0 ? 0.5 : 1.0};
                },
        },
        ItemsRepeater{
            itemsSource = BindOutput{model.words,
                                     [](core::u16_text const& word) {
                                         return Border{Padding{4}, TextBlock{text = word}};
                                     }},
        },
    };
}

}  // namespace
