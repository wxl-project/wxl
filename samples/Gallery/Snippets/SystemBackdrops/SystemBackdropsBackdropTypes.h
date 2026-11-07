// A backdrop is a property of the window, and the kinds of it are objects: Mica (base or
// alt), desktop acrylic, or none.
struct Model {
    enum class Backdrop { None, Mica, MicaAlt, Acrylic };

    TextBlock status {hAlign.center};
    ComboBox backdropBox {
        hAlign.stretch,
        header = u"Current backdrop ",
        ComboBoxItem {content = u"Mica"},
        ComboBoxItem {content = u"MicaAlt"},
        ComboBoxItem {content = u"Acrylic"},
        ComboBoxItem {content = u"None"},
        selectedIndex = 0,
    };
    ComboBox themeBox {
        hAlign.stretch,
        header = u"Window theme ",
        ComboBoxItem {content = u"Use system setting"},
        ComboBoxItem {content = u"Light"},
        ComboBoxItem {content = u"Dark"},
        selectedIndex = 0,
    };
    Grid root {
        RowDefinitions<u"32,*">,
        TitleBar {
            title = u"SystemBackdrop sample window",
            iconSource = ImageIconSource {imageSource = u"Assets/Tiles/BadgeLogo.png"},
        },
        StackPanel {row = 1, hAlign.center, vAlign.center, spacing = 20, status, backdropBox, themeBox},
    };
    Window window {systemBackdrop = MicaBackdrop {}, extendsContentIntoTitleBar = true, content = root};
    Backdrop current = Backdrop::Mica;

    Model() {
        window.appWindow().setIcon(u"Assets/Tiles/GalleryIcon.ico");
        backdropBox.add_onSelectionChanged([this](auto&&...) {
            static constexpr Backdrop kinds[] = {Backdrop::Mica, Backdrop::MicaAlt, Backdrop::Acrylic, Backdrop::None};
            setBackdrop(kinds[std::clamp(backdropBox.selectedIndex(), 0, 3)]);
        });
        themeBox.add_onSelectionChanged([this](auto&&...) {
            static constexpr ElementTheme themes[] = {ElementTheme::Default, ElementTheme::Light, ElementTheme::Dark};
            root.requestedTheme(themes[std::clamp(themeBox.selectedIndex(), 0, 2)]);
            setNoneBackground();
        });
    }

    void setBackdrop(Backdrop type) {
        // Reset to the default colour. If the requested kind is supported, it is set.
        current = Backdrop::None;
        std::u16string note;
        window.noSystemBackdrop(true);

        if (type == Backdrop::Mica || type == Backdrop::MicaAlt) {
            if (MicaController::isSupported()) {
                window.systemBackdrop(
                    MicaBackdrop {kind = type == Backdrop::MicaAlt ? MicaKind::BaseAlt : MicaKind::Base});
                current = type;
            } else {
                // Mica isn't supported. Try Acrylic.
                note += type == Backdrop::Mica ? u"  Mica isn't supported. Trying Acrylic."
                                               : u"  MicaAlt isn't supported. Trying Acrylic.";
                type = Backdrop::Acrylic;
            }
        }
        if (type == Backdrop::Acrylic) {
            if (DesktopAcrylicController::isSupported()) {
                window.systemBackdrop(DesktopAcrylicBackdrop {});
                current = type;
            } else {
                // Acrylic isn't supported, so take the next option, which is the default colour.
                note += u"  Acrylic isn't supported. Switching to default color.";
            }
        }
        status.text(note);
        setNoneBackground();
    }

    // Fixes the background colour not changing when switching between themes.
    void setNoneBackground() {
        if (current == Backdrop::None && themeBox.selectedIndex() != 0) {
            root.background(SolidColorBrush {color = themeBox.selectedIndex() == 1 ? colors.white : colors.black});
        } else {
            root.background(SolidColorBrush {color = colors.transparent});
        }
    }
};

auto example = StackPanel {
    HtmlBlock {
        textWrapping = TextWrapping::WrapWholeWords,
        u"<p>A window can use one of the following system backdrops:<br>"
        u"1. <b>Mica</b> — An opaque material that samples the desktop wallpaper once to tint the window background. Best for main app windows.<br>"
        u"2. <b>Mica Alt</b> — A variant of Mica with stronger tinting. Recommended for apps with a tabbed title bar.<br>"
        u"3. <b>Desktop Acrylic (Base)</b> — A semi-transparent material that shows a blurred view of the content behind the window.<br>"
        u"4. <b>Desktop Acrylic (Thin)</b> — A lighter variant of Desktop Acrylic with more transparency.</p>"
        u"<p><b>Mica vs. Acrylic:</b> Mica is opaque and renders the desktop wallpaper within the window background. Desktop Acrylic is "
        u"semi-transparent and reveals a blurred view of what is behind the window in real-time. Mica is more "
        u"performant because it captures the wallpaper only once, while Acrylic updates continuously.</p>"
        u"<p>There are three backdrop types in the API:</p>"
        u"<ul><li><b>SystemBackdrop</b> — The base class of every backdrop type.</li>"
        u"<li><b>MicaBackdrop</b> — Applies the Mica material. Set the Kind property to switch between Base and Alt.</li>"
        u"<li><b>DesktopAcrylicBackdrop</b> — Applies the Desktop Acrylic material (Base type only).</li></ul>"
        u"<p>All Mica variants require Windows 11 build 22000 or later. In-app acrylic (AcrylicBrush) is a separate "
        u"XAML brush used within UI elements, not a window backdrop.</p>",
    },
    Button {
        Margin {0, 10, 0, 0},
        content = u"Show window",
        onClick += [](Button const&) {
            auto model = std::make_shared<Model>();
            gallery::trackWindow(model->window, model);
            model->window.activate();
        },
    },
};
