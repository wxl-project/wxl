// A complete WXL application: a single function containing the entire UI definition.
// No wWinMain, no Windows App Runtime bootstrap, no Application subclass, and
// no XAML -- WXL manages the lifecycle infrastructure and invokes this entry point.

#include "pch.h"

using namespace wxl;
using namespace wxl::core;
using namespace wxl::dsl;

namespace {

// Presets are their arguments, and these are constants: laid out by the
// compiler, with nothing to run for them at start-up.
namespace presets {
    constexpr Preset centeredAndWrapped{hAlign.center, textWrapping.wrap};
    constexpr Preset allCentered{hAlign.center, vAlign.center};
}

ElementTheme nextTheme(ElementTheme value) {
    constexpr int themeCount = 3;
    return ElementTheme {(int(value) + 1) % themeCount};
}

FluentSymbol themeIcon(ElementTheme value)
{
    switch(value) {
    case ElementTheme::Light:
        return FluentSymbol::Brightness;
    case ElementTheme::Dark:
        return FluentSymbol::MobQuietHours;
    default:
        return FluentSymbol::Settings;
    }
}

std::u16string themeName(ElementTheme value)
{
    switch(value) {
    case ElementTheme::Light:
        return u"Light";
    case ElementTheme::Dark:
        return u"Dark";
    default:
        return u"Default System";
    }
}

}  // namespace

Teardown wxl_launched() {
    // The theme the window requests. It starts at Default, so the window follows the
    // system until the button asks for Light or Dark. Its bindings own what they
    // drive, so the Teardown keeps it as long as the window lives, and a handler on
    // those controls refers to it without owning it.
    auto const themeOwner = make_refcounted<observable<ElementTheme>>(ElementTheme::Default);
    observable<ElementTheme>& theme = *themeOwner;

    auto topBar = Card {
        row = 0,
        hAlign.stretch,
        CornerRadius {2},
        Padding {16, 8},
        Grid {
            HtmlBlock {
                fontFamily = u"Segoe UI",
                fontSize = 28,
                FontWeight {600},
                vAlign.center,
                isTextSelectionEnabled = false,
                // The markup string must be placed last: setters are evaluated from left to right,
                // and the <sub> tag scales relative to the previously applied font size.
                u"<font color=\"#0080ff\">W</font><sub>inUI</sub> "
                u"<font color=\"#ff9d00\">X</font><sub>aml</sub>-"
                u"<font color=\"#00a053\">L</font><sub>ess</sub>",
            },
            StackPanel {
                orientation.horizontal,
                hAlign.right,
                Button {
                    Margin {16, 0},
                    content = BindOutput {theme, [](ElementTheme value) {
                        return SymbolIcon {symbol = themeIcon(nextTheme(value))};
                    }},
                    toolTip = BindOutput {theme, [](ElementTheme value) {
                        return u"Switch theme to " + themeName(nextTheme(value));
                    }},
                    onClick += [&theme] { theme.set(nextTheme(theme.get())); },
                },
                HyperlinkButton {
                    content = u" Github Project Repository ",
                    navigateUri = u"https://github.com/wxl-project/wxl",
                    styles.ButtonBase.TextBlockButton,
                    Padding {3},
                },
            },
        },
    };

    auto card = Card {
        row = 1,
        width = 450,
        height = 310,
        vAlign.center,
        CornerRadius {12},
        Padding {0},
        background = brushes.SolidBackgroundFillColor.Secondary,
        Grid {
            RowDefinitions<u"auto,*,auto">,
            StackPanel {
                row = 0,
                orientation.horizontal,
                Padding {110, 0},
                Image {
                    source = u"Assets/wxl.png",
                    Margin {8, 0},
                    width = 78,
                    height = 73.3333333,
                },
                TextBlock {
                    text = u"WXL",
                    styles.TextBlock.Header,
                    vAlign.center,
                    hAlign.center,
                    FontWeight {600},
                },
            },
            Rows {
                row = 1,
                background = brushes.Card.BackgroundFillColor.Default,
                TextBlock {
                    u"A modern C++23 library for declarative, Flutter-style WinUI development.",
                    Margin { 16, 0 },
                    vAlign.bottom,
                    styles.TextBlock.Subtitle,
                    presets::centeredAndWrapped,
                },
                TextBlock {
                    u"Leveraging C++ modules to eliminate heavy WinRT bloat, "
                    u"it restores simplicity and control to native C++ GUI development.",
                    Margin { 16, 14 },
                    vAlign.center,
                    presets::centeredAndWrapped,
                },
            },
            Button {
                row = 2,
                Margin {0, 12},
                content = u"Click",
                onClick += [](Button const& self) { self.content(u"Thank You!"); },
                hAlign.center,
            },
        },
    };

    auto window = Window {
        title = u"Hello from WXL",
        minSize = {500, 420},
        // On the window, not on its content: the frame follows it too.
        requestedTheme = BindOutput {theme},
        Grid {
            RowDefinitions<u"auto,*,auto">,
            background = brushes.SolidBackgroundFillColor.Base,
            topBar,
            card,
        },
    };

    auto appWindow = window.appWindow();
    appWindow.resize({800, 600});
    window.activate();

    return [themeOwner](TeardownReason) {};
}
