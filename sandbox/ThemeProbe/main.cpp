// Проба темы окна.
//
// Вопросы:
//   1. Что отдаёт Window.Content пустого окна и есть ли предок над содержимым сразу после
//      присваивания, до раскладки.
//   2. Какие элементы стоят над Content (тип, имя, тема) и совпадает ли XamlRoot.Content
//      с Window.Content.
//   3. Наследует ли содержимое тему, заданную на самом верхнем FrameworkElement над ним,
//      и приходит ли ActualThemeChanged.
//   4. Тот ли это объект после замены Content, после Content(nullptr) и после возврата
//      прежнего содержимого; сохраняет ли он свою тему.
//   5. Сохраняет ли содержимое со своей явной темой её под тёмным предком.
//   6. Какую тему видят Flyout и ContentDialog и что стоит над ними.
//
// Смена системной темы здесь не проверяется: она меняет настройки пользователя.
// Ответ -- theme-probe.txt рядом с исполняемым файлом; окно закрывается само.

#include "platform.h"

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#include <cstdarg>
#include <cstdio>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "launch.h"

namespace {

namespace wf = winrt::Windows::Foundation;
namespace xaml = winrt::Microsoft::UI::Xaml;
namespace controls = winrt::Microsoft::UI::Xaml::Controls;
namespace media = winrt::Microsoft::UI::Xaml::Media;
namespace dispatching = winrt::Microsoft::UI::Dispatching;

FILE* logFile = nullptr;
ULONGLONG startTick = 0;

void say(const char* format, ...) {
    if (!logFile) return;
    std::fprintf(logFile, "%6llu ", ::GetTickCount64() - startTick);
    va_list args;
    va_start(args, format);
    std::vfprintf(logFile, format, args);
    va_end(args);
    std::fputc('\n', logFile);
    std::fflush(logFile);
}

const char* themeName(xaml::ElementTheme theme) {
    switch (theme) {
        case xaml::ElementTheme::Default: return "Default";
        case xaml::ElementTheme::Light: return "Light";
        case xaml::ElementTheme::Dark: return "Dark";
    }
    return "?";
}

bool same(wf::IInspectable const& a, wf::IInspectable const& b) {
    if (!a || !b) return !a && !b;
    return winrt::get_abi(a.as<wf::IUnknown>()) == winrt::get_abi(b.as<wf::IUnknown>());
}

const char* yes(bool value) { return value ? "yes" : "no"; }

void describe(int depth, xaml::DependencyObject const& node) {
    std::string const type = winrt::to_string(winrt::get_class_name(node));
    if (auto const element = node.try_as<xaml::FrameworkElement>()) {
        say("    %d %s name='%s' requested=%s actual=%s loaded=%s", depth, type.c_str(),
            winrt::to_string(element.Name()).c_str(), themeName(element.RequestedTheme()),
            themeName(element.ActualTheme()), yes(element.IsLoaded()));
    } else {
        say("    %d %s (not a FrameworkElement)", depth, type.c_str());
    }
}

// Идёт от элемента вверх по визуальному дереву; возвращает самый верхний FrameworkElement.
xaml::FrameworkElement walkUp(const char* label, xaml::DependencyObject const& from) {
    say("  %s:", label);
    xaml::FrameworkElement top{nullptr};
    int depth = 0;
    for (auto node = from; node; node = media::VisualTreeHelper::GetParent(node)) {
        describe(depth++, node);
        if (auto const element = node.try_as<xaml::FrameworkElement>()) top = element;
    }
    if (depth == 0) say("    (nothing)");
    return top;
}

controls::Grid makeContent(const wchar_t* name, xaml::ElementTheme theme) {
    controls::Grid grid;
    grid.Name(name);
    grid.RequestedTheme(theme);
    controls::StackPanel panel;
    controls::TextBlock text;
    text.Text(name);
    panel.Children().Append(text);
    controls::Button button;
    button.Name(std::wstring(name) + L".button");
    button.Content(winrt::box_value(L"button"));
    panel.Children().Append(button);
    grid.Children().Append(panel);
    return grid;
}

controls::Button buttonOf(controls::Grid const& grid) {
    return grid.Children().GetAt(0).as<controls::StackPanel>().Children().GetAt(1).as<controls::Button>();
}

struct Probe {
    xaml::Window window{nullptr};
    controls::Grid a{nullptr}, b{nullptr}, c{nullptr};
    xaml::FrameworkElement top{nullptr};
    controls::Flyout flyout{nullptr};
    controls::TextBlock flyoutText{nullptr};
    controls::ContentDialog dialog{nullptr};
    std::vector<std::pair<int, std::function<void()>>> steps;
    size_t next = 0;
    dispatching::DispatcherQueueTimer timer{nullptr};
};

Probe* probe = nullptr;

void step(int delayMs, std::function<void()> action) { probe->steps.emplace_back(delayMs, std::move(action)); }

void runNext() {
    if (probe->next == probe->steps.size()) return;
    auto& [delay, action] = probe->steps[probe->next];
    probe->timer.Interval(std::chrono::milliseconds{delay});
    probe->timer.Start();
}

void fire() {
    probe->timer.Stop();
    auto const action = probe->steps[probe->next++].second;
    try {
        action();
    } catch (winrt::hresult_error const& error) {
        say("  ERROR %08x %s", static_cast<unsigned>(error.code().value), winrt::to_string(error.message()).c_str());
    }
    runNext();
}

void watch(const char* label, xaml::FrameworkElement const& element) {
    std::string const name = label;
    element.ActualThemeChanged([name](xaml::FrameworkElement const& self, wf::IInspectable const&) {
        say("  event: ActualThemeChanged on %s -> %s", name.c_str(), themeName(self.ActualTheme()));
    });
}

void reportTop(const char* label, xaml::FrameworkElement const& now) {
    say("  %s: same top as before = %s; old top requested=%s actual=%s loaded=%s", label, yes(same(now, probe->top)),
        themeName(probe->top.RequestedTheme()), themeName(probe->top.ActualTheme()), yes(probe->top.IsLoaded()));
}

void plan() {
    auto& p = *probe;

    step(700, [&p] {
        say("Q2. ancestors of a laid-out Content");
        p.top = walkUp("from a", p.a);
        say("  XamlRoot is %s", p.a.XamlRoot() ? "set" : "null");
        if (auto const root = p.a.XamlRoot()) {
            say("  XamlRoot.Content is Window.Content = %s; is the top = %s", yes(same(root.Content(), p.window.Content())),
                yes(same(root.Content(), p.top)));
        }
        say("  top is a itself = %s", yes(same(p.top, p.a)));
        watch("top", p.top);
        watch("a", p.a);
    });
    step(0, [&p] {
        say("Q3. top.RequestedTheme(Dark)");
        p.top.RequestedTheme(xaml::ElementTheme::Dark);
    });
    step(600, [&p] {
        say("  after: top requested=%s actual=%s; a actual=%s", themeName(p.top.RequestedTheme()),
            themeName(p.top.ActualTheme()), themeName(p.a.ActualTheme()));
    });
    step(0, [&p] {
        say("Q4. Content(b), b without a theme of its own");
        p.b = makeContent(L"b", xaml::ElementTheme::Default);
        watch("b", p.b);
        p.window.Content(p.b);
        reportTop("right after the assignment", walkUp("from b, synchronously", p.b));
    });
    step(700, [&p] {
        reportTop("after layout", walkUp("from b", p.b));
        say("  b actual=%s; a (taken out) actual=%s loaded=%s parent=%s", themeName(p.b.ActualTheme()),
            themeName(p.a.ActualTheme()), yes(p.a.IsLoaded()), media::VisualTreeHelper::GetParent(p.a) ? "set" : "null");
    });
    step(0, [&p] {
        say("Q4. Content(nullptr)");
        p.window.Content(nullptr);
    });
    step(600, [&p] {
        say("  Window.Content is %s", p.window.Content() ? "set" : "null");
        say("  old top: requested=%s loaded=%s XamlRoot=%s", themeName(p.top.RequestedTheme()), yes(p.top.IsLoaded()),
            p.top.XamlRoot() ? "set" : "null");
        walkUp("from the old top", p.top);
    });
    step(0, [&p] {
        say("Q5. Content(c), c requests Light");
        p.c = makeContent(L"c", xaml::ElementTheme::Light);
        watch("c", p.c);
        p.window.Content(p.c);
    });
    step(700, [&p] {
        reportTop("after layout", walkUp("from c", p.c));
        say("  c requested=%s actual=%s", themeName(p.c.RequestedTheme()), themeName(p.c.ActualTheme()));
    });
    step(0, [&p] {
        say("Q4. Content(b) again");
        p.window.Content(p.b);
    });
    step(700, [&p] {
        reportTop("after layout", walkUp("from b", p.b));
        say("  b actual=%s", themeName(p.b.ActualTheme()));
    });
    step(0, [&p] {
        say("Q6. Flyout at b's button");
        p.flyout = controls::Flyout{};
        p.flyoutText = controls::TextBlock{};
        p.flyoutText.Name(L"flyout.text");
        p.flyoutText.Text(L"flyout");
        p.flyout.Content(p.flyoutText);
        p.flyout.ShowAt(buttonOf(p.b));
    });
    step(700, [&p] {
        auto const flyoutTop = walkUp("from the flyout's text", p.flyoutText);
        say("  flyout text actual=%s; its top is the window's top = %s", themeName(p.flyoutText.ActualTheme()),
            yes(same(flyoutTop, p.top)));
        p.flyout.Hide();
    });
    step(300, [&p] {
        say("Q6. ContentDialog over b");
        p.dialog = controls::ContentDialog{};
        p.dialog.Name(L"dialog");
        p.dialog.XamlRoot(p.b.XamlRoot());
        p.dialog.Title(winrt::box_value(L"dialog"));
        p.dialog.Content(winrt::box_value(L"content"));
        p.dialog.CloseButtonText(L"Close");
        p.dialog.ShowAsync();
    });
    step(900, [&p] {
        auto const dialogTop = walkUp("from the dialog", p.dialog);
        say("  dialog requested=%s actual=%s; its top is the window's top = %s", themeName(p.dialog.RequestedTheme()),
            themeName(p.dialog.ActualTheme()), yes(same(dialogTop, p.top)));
        p.dialog.Hide();
    });
    step(300, [&p] {
        say("Q3. top.RequestedTheme(Light), then Default");
        p.top.RequestedTheme(xaml::ElementTheme::Light);
    });
    step(500, [&p] {
        say("  b actual=%s", themeName(p.b.ActualTheme()));
        p.top.RequestedTheme(xaml::ElementTheme::Default);
    });
    step(500, [&p] {
        say("  b actual=%s (Default: the application's theme)", themeName(p.b.ActualTheme()));
        say("done");
        std::fclose(logFile);
        logFile = nullptr;
        p.window.Close();
        xaml::Application::Current().Exit();
    });
}

}  // namespace

wxl::Teardown wxl_launched() {
    wchar_t path[MAX_PATH] = {};
    ::GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring logPath{path};
    logPath.resize(logPath.find_last_of(L'\\') + 1);
    logPath += L"theme-probe.txt";
    logFile = ::_wfopen(logPath.c_str(), L"w");
    startTick = ::GetTickCount64();

    probe = new Probe{};
    auto& p = *probe;

    say("Q1. an empty window");
    p.window = xaml::Window{};
    p.window.Title(L"ThemeProbe");
    p.window.AppWindow().Resize({640, 480});
    say("  Window.Content of a new window is %s", p.window.Content() ? "set" : "null");

    p.a = makeContent(L"a", xaml::ElementTheme::Default);
    p.window.Content(p.a);
    walkUp("from a, synchronously after Content(a), before Activate", p.a);
    p.window.Activate();
    walkUp("from a, synchronously after Activate", p.a);

    p.timer = dispatching::DispatcherQueue::GetForCurrentThread().CreateTimer();
    p.timer.IsRepeating(false);
    p.timer.Tick([](auto&&, auto&&) { fire(); });
    plan();
    runNext();
    return {};
}
