// The model owns the six boxes and the bar, which the handler reads by address.
struct Model {
    NumberBox clientWidth = dimension(u"Width", u"Client width in DIPs", u"WindowingClientWidth", 640, 0, 0);
    NumberBox clientHeight = dimension(u"Height", u"Client height in DIPs", u"WindowingClientHeight", 480, 0, 1);
    NumberBox minimumWidth = dimension(u"MinWidth", u"Minimum client width in DIPs", u"WindowingMinimumWidth", 320, 1, 0);
    NumberBox minimumHeight = dimension(u"MinHeight", u"Minimum client height in DIPs", u"WindowingMinimumHeight", 240, 1, 1);
    NumberBox maximumWidth = dimension(u"MaxWidth", u"Maximum client width in DIPs", u"WindowingMaximumWidth", 960, 2, 0);
    NumberBox maximumHeight = dimension(u"MaxHeight", u"Maximum client height in DIPs", u"WindowingMaximumHeight", 720, 2, 1);

    InfoBar validation {
        isClosable = true,
        isOpen = false,
        severity = InfoBarSeverity::Error,
        title = u"Check the window dimensions",
    };

    static NumberBox dimension(char16_t const* headerText, char16_t const* automationText, char16_t const* id, double initial, int boxRow, int boxColumn) {
        return NumberBox {
            hAlign.stretch,
            automationId = id,
            automationName = automationText,
            header = headerText,
            row = boxRow,
            column = boxColumn,
            minimum = 1.0,
            maximum = 10000.0,
            smallChange = 10.0,
            spinButtonPlacementMode = NumberBoxSpinButtonPlacementMode::Compact,
            validationMode = NumberBoxValidationMode::InvalidInputOverwritten,
            value = initial,
        };
    }

    static std::u16string description(double clientWidth, double clientHeight, double minClientWidth, double maxClientWidth, double minClientHeight,
                                      double maxClientHeight) {
        auto number = [](double amount) { return core::to_u16(amount, std::chars_format::fixed, 0); };
        std::u16string line = u"Initial client area: ";
        line += number(clientWidth).plain();
        line += u" by ";
        line += number(clientHeight).plain();
        line += u" DIPs. Width is constrained to ";
        line += number(minClientWidth).plain();
        line += u" to ";
        line += number(maxClientWidth).plain();
        line += u" DIPs and height to ";
        line += number(minClientHeight).plain();
        line += u" to ";
        line += number(maxClientHeight).plain();
        line += u" DIPs. Try resizing this window.";
        return line;
    }

    void open() {
        double const widthValue = clientWidth.value();
        double const heightValue = clientHeight.value();
        double const minWidthValue = minimumWidth.value();
        double const minHeightValue = minimumHeight.value();
        double const maxWidthValue = maximumWidth.value();
        double const maxHeightValue = maximumHeight.value();

        if (!std::isfinite(widthValue) || !std::isfinite(heightValue) || !std::isfinite(minWidthValue) || !std::isfinite(minHeightValue) ||
            !std::isfinite(maxWidthValue) || !std::isfinite(maxHeightValue)) {
            validation.message(u"Enter a value for every dimension.");
            validation.isOpen(true);
            return;
        }
        if (minWidthValue > widthValue || widthValue > maxWidthValue || minHeightValue > heightValue || heightValue > maxHeightValue) {
            validation.message(u"Width and Height must be within their minimum and maximum limits.");
            validation.isOpen(true);
            return;
        }
        validation.isOpen(false);

        Window window {
            title = u"Window client size and constraints (experimental)",
            systemBackdrop = MicaBackdrop {},
            content = TextBlock {
                Margin {24},
                textWrapping = TextWrapping::Wrap,
                description(widthValue, heightValue, minWidthValue, maxWidthValue, minHeightValue, maxHeightValue),
            },
        };
        // The size properties of Window are experimental: a runtime of the stable channel
        // answers the first of them with "no such interface", and the window is not shown.
        try {
            window.minWidth(minWidthValue);
            window.minHeight(minHeightValue);
            window.maxWidth(maxWidthValue);
            window.maxHeight(maxHeightValue);
            window.width(widthValue);
            window.height(heightValue);
        } catch (...) {
            window.close();
            validation.message(u"The installed Windows App SDK runtime does not have the experimental Window size properties.");
            validation.isOpen(true);
            return;
        }
        if (auto presenter = window.appWindow().presenter().try_as<OverlappedPresenter>()) {
            presenter.isMaximizable(false);
        }
        gallery::trackWindow(window);
        // Activate applies the pending Window size and constraints before the first show.
        window.activate();
    }
};
auto const model = gallery::hold<Model>();

auto example = StackPanel {
    spacing = 12,
    TextBlock {
        textWrapping = TextWrapping::Wrap,
        u"Configure the initial, minimum, and maximum client-area dimensions in device-independent pixels (DIPs). "
        u"The initial size must be within the minimum and maximum limits.",
    },
    model->validation,
    Button {
        automationId = u"WindowingOpenConfiguredWindow",
        automationName = u"Open a window with the configured client size and constraints",
        content = u"Open configured window",
        onClick += [model](Button const&) { model->open(); },
    },
};

auto options = Grid {
    columnSpacing = 12,
    rowSpacing = 12,
    ColumnDefinitions<u"*,*">,
    RowDefinitions<u"auto,auto,auto">,
    model->clientWidth,
    model->clientHeight,
    model->minimumWidth,
    model->minimumHeight,
    model->maximumWidth,
    model->maximumHeight,
};