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

ElementTheme themeOf(bool dark) {
    return dark ? ElementTheme::Dark : ElementTheme::Light;
}

// The button shows where it leads: the icon and the name of the other theme.
FluentSymbol switchIcon(bool dark) {
    return dark ? FluentSymbol::Brightness : FluentSymbol::MobQuietHours;
}

std::u16string switchTip(bool dark) {
    return dark ? u"Switch theme to Light" : u"Switch theme to Dark";
}

}  // namespace

Teardown wxl_launched() {
    // Whether the window is dark. It takes the system's theme before the window shows,
    // and the button flips it. Its bindings own what they drive, so the Teardown keeps
    // it as long as the window lives, and a handler on those controls refers to it
    // without owning it.
    auto const darkOwner = make_refcounted<observable<bool>>();
    observable<bool>& darkTheme = *darkOwner;

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
                    content = BindOutput {darkTheme, [](bool dark) {
                        return SymbolIcon {symbol = switchIcon(dark)};
                    }},
                    toolTip = BindOutput {darkTheme, switchTip},
                    onClick += [&darkTheme] { darkTheme.set(!darkTheme.get()); },
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
        Grid {
            RowDefinitions<u"auto,*,auto">,
            background = brushes.SolidBackgroundFillColor.Base,
            topBar,
            card,
        },
    };

    // The window opens in the system's theme, and from then on shows the field. On the
    // window, not on its content: the frame follows it too.
    darkTheme.set(WindowNative::actualTheme(window) == ElementTheme::Dark);
    Apply {window, requestedTheme = BindOutput {darkTheme, themeOf}};

    auto appWindow = window.appWindow();
    appWindow.resize({800, 600});
    window.activate();

    return [darkOwner](TeardownReason) {};
}
