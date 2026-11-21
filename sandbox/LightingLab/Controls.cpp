// Строки панели настроек: подпись, значение и контрол, привязанный к полю модели.

#include "Lab.h"

using namespace wxl;

namespace {

// До двух знаков: шаг ползунков не мельче сотой.
hstring number(double value) {
    return hstring {core::to_u16(value, std::chars_format::fixed, 2)};
}

template <class Value>
FrameworkElement slider(char16_t const* title, core::observable<Value>& field, double low, double high,
                        double step) {
    using namespace wxl::dsl;
    return Grid {
        ColumnDefinitions<u"*,auto">,
        RowDefinitions<u"auto,auto">,
        TextBlock {title},
        TextBlock {column = 1, opacity = 0.65, text = BindOutput {field, [](double amount) { return number(amount); }}},
        Slider {
            row = 1,
            columnSpan = 2,
            automationName = title,
            minimum = low,
            maximum = high,
            stepFrequency = step,
            smallChange = step,
            value = Bind {field},
        },
    };
}

}  // namespace

FrameworkElement lab::sliderRow(char16_t const* title, core::observable<double>& field, double low, double high,
                                double step) {
    return slider(title, field, low, high, step);
}

FrameworkElement lab::sliderRow(char16_t const* title, core::observable<float>& field, double low, double high,
                                double step) {
    return slider(title, field, low, high, step);
}

FrameworkElement lab::choiceRow(char16_t const* title, core::observable<int>& field,
                                std::span<char16_t const* const> names) {
    using namespace wxl::dsl;
    auto box = ComboBox {header = title, hAlign.stretch};
    for (auto const option : names) {
        box.items().append(ComboBoxItem {content = option});
    }
    // После пунктов: выбранный номер у пустого списка не держится.
    Apply {box, selectedIndex = Bind {field}};
    return box;
}

FrameworkElement lab::toggleRow(char16_t const* title, core::observable<bool>& field) {
    using namespace wxl::dsl;
    return ToggleSwitch {header = title, isOn = Bind {field}};
}

// Флажок пишет в поле сам: его IsChecked -- значение с тремя состояниями, и
// двусторонней привязки к bool у него нет.
FrameworkElement lab::checkRow(char16_t const* title, core::observable<bool>& field) {
    using namespace wxl::dsl;
    return CheckBox {
        content = title,
        isChecked = field.get(),
        onChecked += [&field] { field.set(true); },
        onUnchecked += [&field] { field.set(false); },
    };
}

FrameworkElement lab::colorRow(char16_t const* title, core::observable<Color>& field) {
    using namespace wxl::dsl;
    return Grid {
        ColumnDefinitions<u"*,auto">,
        TextBlock {title, vAlign.center},
        SplitButton {
            column = 1,
            automationName = title,
            content = Border {
                width = 48,
                height = 24,
                Margin {-11, -6},
                CornerRadius {4, 0, 0, 4},
                background = BindOutput {field, [](Color c) { return SolidColorBrush {color = c}; }},
            },
            flyout = Flyout {
                placement = FlyoutPlacementMode::Bottom,
                content = ColorPicker {
                    color = Bind {field},
                    colorSpectrumShape = ColorSpectrumShape::Ring,
                    isAlphaEnabled = false,
                    isColorChannelTextInputVisible = false,
                    isHexInputVisible = true,
                    isMoreButtonVisible = false,
                },
            },
        },
    };
}

FrameworkElement lab::noteRow(char16_t const* words) {
    using namespace wxl::dsl;
    return TextBlock {words, textWrapping = TextWrapping::Wrap, opacity = 0.65};
}

FrameworkElement lab::statusRow(core::observable<hstring>& field) {
    using namespace wxl::dsl;
    return TextBlock {text = BindOutput {field}, textWrapping = TextWrapping::Wrap, isTextSelectionEnabled = true};
}

FrameworkElement lab::group(char16_t const* title, bool open, std::initializer_list<FrameworkElement> rows) {
    using namespace wxl::dsl;
    auto panel = StackPanel {spacing = 10.0};
    for (auto const& record : rows) {
        panel.children().append(record);
    }
    return Expander {
        hAlign.stretch,
        horizontalContentAlignment = HorizontalAlignment::Stretch,
        header = title,
        isExpanded = open,
        content = panel,
    };
}

FrameworkElement lab::settingsPanel(std::initializer_list<FrameworkElement> groups) {
    using namespace wxl::dsl;
    auto panel = StackPanel {spacing = 8.0, Margin {12}};
    for (auto const& record : groups) {
        panel.children().append(record);
    }
    return panel;
}

hstring lab::accepted(char16_t const* what) {
    std::u16string line {what};
    line += u": принято";
    return hstring {line};
}

hstring lab::refusal(char16_t const* what, std::int32_t code, char16_t const* why) {
    std::u16string line {what};
    line += u": отказ 0x";
    core::append_number(line, static_cast<std::uint32_t>(code), 16);
    if (*why != 0) {
        line += u", ";
        line += why;
    }
    return hstring {line};
}

hstring lab::refusal(char16_t const* what, char16_t const* why) {
    std::u16string line {what};
    line += u": отказ, ";
    line += why;
    return hstring {line};
}
