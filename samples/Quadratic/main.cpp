// Квадратное уравнение: три коэффициента на входе, дискриминант и корни на выходе.
// Модель — поля с оповещением, интерфейс — одно выражение, привязанное к ним.

#include "Bind.h"
#include "Card.h"
#include "CompositionWindow.h"
#include "GlassEffect.h"
#include "launch.h"
#include "ui.h"

import wxl.core;

using namespace wxl;
using namespace wxl::core;
using namespace wxl::dsl;

// Девять значащих цифр — у double и у обеих частей complex одинаково.
u16_text toText(auto value) {
    return format(u"{:.9g}", value);
}

struct Answer {
    u16_text D, x1, x2;
};

// Пустое поле NumberBox — это NaN, и текст, который числом ещё не стал, — тоже.
constexpr double blank = std::numeric_limits<double>::quiet_NaN();

// Корни через q = −(b + sign(b)·√D)/2 как q/a и c/q: без вычитания близких чисел,
// которое съедает точность меньшего по модулю корня. При a = 0 уравнение линейное.
Answer solve(double a, double b, double c) {
    u16_text const none{u"—"};

    if (std::isnan(a) || std::isnan(b) || std::isnan(c))
        return {none, none, none};

    if (a == 0) {
        if (b != 0)
            return {none, toText(-c / b), none};

        if (c == 0)
            return {none, u16_text{u"ℝ"}, none};

        return {none, u16_text{u"∅"}, none};
    }

    double const d = b * b - 4 * a * c;
    std::complex<double> first, second;

    if (d < 0) {
        // Комплексные корни — сопряжённая пара
        second = {-b / (2 * a), std::sqrt(-d) / (2 * std::abs(a))};
        first = std::conj(second);
    } else {
        // Формула Мюллера для сохранения точности
        double const q = -0.5 * (b + std::copysign(std::sqrt(d), b));
        double const one = q / a;
        // Теорема Виета
        double const other = q == 0 ? one : c / q;
        first = std::min(one, other);
        second = std::max(one, other);
    }

    return {toText(d), toText(first), toText(second)};
}

// Коэффициенты пишут поля ввода, ответы показывают поля вывода. Любая правка
// коэффициента пересчитывает ответы, пока все три — числа.
class Equation : public noncopyable
{
public:
    observable<Answer> answer;
    observable<double> a{blank}, b{blank}, c{blank};

    Equation() {
        answer.follow(a, b, c, solve);
    }
};

namespace presets {
    inline constexpr Preset common {
        fontSize = 18,
        hAlign.center,
        vAlign.center,
    };

    inline constexpr Preset txt {
        common, // Пресеты могут вкладываться друг в друга
        FontWeight {600},
        foreground = colors.blue,
    };

    inline constexpr Preset input {
        common,
        width = 200,
    };

    inline constexpr Preset output {
        common,
        width = 300,
        isReadOnly = true,
    };
}

wxl::Teardown wxl_launched() {
    // Время жизни модели требуется продлить
    auto const model = std::make_shared<Equation>();
    Equation & eq = *model;

    auto window = CompositionWindow {
        title = u"WXL Quadratic",
        minSize = {910, 390},
        background = BackgroundImage {u"Assets/GlassBackdrop.png", BackgroundFill::UniformToFill},
        Card {
            GlassEffect {blurRadius = 24.0f},
            background = rgba(255, 255, 255, 0.35),
            hAlign.center,
            vAlign.center,
            StackPanel {
                spacing = 32,
                TextBlock {
                    u"Решение квадратного уравнения",
                    styles.TextBlock.Subtitle,
                    hAlign.center,
                },
                StackPanel {
                    orientation.horizontal,
                    spacing = 10,
                    NumberBox {
                        presets::input,
                        placeholderText = u"a",
                        intermediateValue = BindInput {eq.a},
                        onLoaded += [](NumberBox const& control) {
                            control.focus(FocusState::Programmatic);
                        },
                    },
                    TextBlock {presets::txt, u"· x² +"},
                    NumberBox {
                        presets::input,
                        placeholderText = u"b",
                        intermediateValue = BindInput {eq.b},
                    },
                    TextBlock {presets::txt, u"· x +"},
                    NumberBox {
                        presets::input,
                        placeholderText = u"c",
                        intermediateValue = BindInput {eq.c},
                    },
                    TextBlock {presets::txt, u"= 0"},
                },
                StackPanel {
                    orientation.horizontal,
                    spacing = 10,
                    TextBlock {presets::txt, u"D ="},
                    TextBox {
                        presets::output,
                        text = BindOutput {eq.answer, &Answer::D},
                    },
                },
                StackPanel {
                    orientation.horizontal,
                    spacing = 10,
                    TextBlock {presets::txt, u"x₁ ="},
                    TextBox {
                        presets::output,
                        text = BindOutput {eq.answer, &Answer::x1},
                    },
                    TextBlock {presets::txt, u",", Margin {0, 0, 16, 0}},
                    TextBlock {presets::txt, u"x₂ ="},
                    TextBox {
                        presets::output,
                        text = BindOutput {eq.answer, &Answer::x2},
                    },
                },
            },
        }
    };

    window.appWindow().resize({1024, 480});
    window.activate();

    // Продлеваем время жизни модели
    return [model](TeardownReason) {};
}
