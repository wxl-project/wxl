// Опыт «Свет композитора: прожектор за указателем».
//
// Клавиши -- кнопки XAML. Обычные носят RevealEffect из библиотеки: узкий
// HoverLight кладёт пятно на лицо клавиши под указателем, широкий BorderLight
// высвечивает рамки соседних. Правая колонка -- Button3DEffect: свет у него
// свой, и отдаёт он его слабее; его источники написаны в скобках эффекта
// голыми значениями, с теми же diffuseAmount, что у эффекта по умолчанию.
// Ползунки панели пишут в сами источники: настройка действует на уже
// освещённые клавиши. Лампы у обеих пар настраиваются вместе, отдача света --
// порознь.

#include "Lab.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

struct Reveal {
    core::observable<float> hoverSize {140.0f};
    core::observable<float> hoverHeight {120.0f};
    core::observable<float> hoverIntensity {1.0f};
    core::observable<float> hoverDiffuse {0.69f};
    core::observable<float> keyHoverDiffuse {0.5f};

    core::observable<float> borderSize {340.0f};
    core::observable<float> borderHeight {120.0f};
    core::observable<float> borderIntensity {1.0f};
    core::observable<float> borderDiffuse {1.24f};
    core::observable<float> keyBorderDiffuse {0.9f};
    core::observable<float> thickness {1.5f};
    // Пока ползунок не тронут, скругление рамки у каждой клавиши своё.
    core::observable<double> corner {4.0};

    core::observable<Color> lightColor {rgb(255, 255, 255)};
    core::observable<float> constantAttenuation {1.0f};
    core::observable<float> linearAttenuation {0.0f};

    core::observable<hstring> status;

    HoverLight hover;
    BorderLight border;
    RevealEffect reveal {hover, border};

    HoverLight keyHover {diffuseAmount = 0.5f};
    BorderLight keyBorder {diffuseAmount = 0.9f};
    Button3DEffect relief {
        foreground = rgb(255, 227, 180),
        background = rgba(176, 98, 30, 0.933),
        shadow = 0.4,
        emboss = -0.3,
        keyHover,
        keyBorder,
    };

    Reveal() {
        tune(hoverSize, u"HoverLight size", [this](float value) {
            hover.size(value);
            keyHover.size(value);
        });
        tune(hoverHeight, u"HoverLight height", [this](float value) {
            hover.height(value);
            keyHover.height(value);
        });
        tune(hoverIntensity, u"HoverLight intensity", [this](float value) {
            hover.intensity(value);
            keyHover.intensity(value);
        });
        tune(hoverDiffuse, u"HoverLight diffuseAmount", [this](float value) { hover.diffuseAmount(value); });
        tune(keyHoverDiffuse, u"HoverLight diffuseAmount, Button3DEffect",
             [this](float value) { keyHover.diffuseAmount(value); });

        tune(borderSize, u"BorderLight size", [this](float value) {
            border.size(value);
            keyBorder.size(value);
        });
        tune(borderHeight, u"BorderLight height", [this](float value) {
            border.height(value);
            keyBorder.height(value);
        });
        tune(borderIntensity, u"BorderLight intensity", [this](float value) {
            border.intensity(value);
            keyBorder.intensity(value);
        });
        tune(borderDiffuse, u"BorderLight diffuseAmount", [this](float value) { border.diffuseAmount(value); });
        tune(keyBorderDiffuse, u"BorderLight diffuseAmount, Button3DEffect",
             [this](float value) { keyBorder.diffuseAmount(value); });
        tune(thickness, u"BorderLight strokeThickness", [this](float value) {
            border.strokeThickness(value);
            keyBorder.strokeThickness(value);
        });
        tune(corner, u"BorderLight cornerRadius", [this](double value) {
            border.cornerRadius(CornerRadius {value});
            keyBorder.cornerRadius(CornerRadius {value});
        });

        tune(lightColor, u"color", [this](Color value) {
            hover.color(value);
            border.color(value);
            keyHover.color(value);
            keyBorder.color(value);
        });
        tune(constantAttenuation, u"constantAttenuation", [this](float value) {
            hover.constantAttenuation(value);
            border.constantAttenuation(value);
            keyHover.constantAttenuation(value);
            keyBorder.constantAttenuation(value);
        });
        tune(linearAttenuation, u"linearAttenuation", [this](float value) {
            hover.linearAttenuation(value);
            border.linearAttenuation(value);
            keyHover.linearAttenuation(value);
            keyBorder.linearAttenuation(value);
        });
    }

    // Настройка пишется в источник света при каждом изменении поля; чем
    // ответила система, видно в строке состояния.
    template <class Value, class Write>
    void tune(core::observable<Value>& field, char16_t const* what, Write write) {
        field.on_change([this, what, write](Value const& value) noexcept {
            lab::attempt(status, what, [&] { write(value); });
        });
    }
};

}  // namespace

lab::Experiment lab::revealPage() {
    auto const model = std::make_shared<Reveal>();

    static constexpr char16_t const* labels[] = {
        u"7", u"8", u"9", u"÷", u"4", u"5", u"6", u"×", u"1", u"2", u"3", u"−", u"0", u",", u"=", u"+",
    };

    auto pad = Grid {
        width = 440,
        height = 440,
        hAlign.center,
        vAlign.center,
        requestedTheme = ElementTheme::Dark,
        background = SolidColorBrush {color = rgb(30, 31, 34)},
        CornerRadius {12},
        Padding {10},
        RowDefinitions<u"*,*,*,*">,
        ColumnDefinitions<u"*,*,*,*">,
    };

    for (int index = 0; index < 16; ++index) {
        if (index % 4 == 3) {
            pad.children().append(Button {
                row = index / 4,
                column = index % 4,
                hAlign.stretch,
                vAlign.stretch,
                Margin {5},
                CornerRadius {8},
                fontSize = 22,
                content = labels[index],
                model->relief,
            });
        } else {
            pad.children().append(Button {
                row = index / 4,
                column = index % 4,
                hAlign.stretch,
                vAlign.stretch,
                Margin {5},
                fontSize = 22,
                content = labels[index],
                model->reveal,
            });
        }
    }

    auto settings = lab::settingsPanel({
        lab::noteRow(u"Обычные клавиши носят RevealEffect {HoverLight, BorderLight}. Правая колонка -- "
                     u"Button3DEffect: свет у него свой, источники написаны в его скобках голыми значениями и "
                     u"отдают свет слабее. Свет виден, пока указатель над содержимым окна. Нажатие клавиши -- "
                     u"вспышка пятна."),
        lab::statusRow(model->status),
        lab::group(u"HoverLight: пятно на клавише", true,
                   {
                       lab::sliderRow(u"Пятно: size", model->hoverSize, 10.0, 800.0, 1.0),
                       lab::sliderRow(u"Пятно: height", model->hoverHeight, 10.0, 600.0, 1.0),
                       lab::sliderRow(u"Пятно: intensity", model->hoverIntensity, 0.0, 6.0, 0.01),
                       lab::sliderRow(u"Пятно: diffuseAmount", model->hoverDiffuse, 0.0, 3.0, 0.01),
                       lab::sliderRow(u"Пятно: diffuseAmount у Button3DEffect", model->keyHoverDiffuse, 0.0, 3.0,
                                      0.01),
                   }),
        lab::group(u"BorderLight: свет на рамках", true,
                   {
                       lab::sliderRow(u"Рамки: size", model->borderSize, 10.0, 1200.0, 1.0),
                       lab::sliderRow(u"Рамки: height", model->borderHeight, 10.0, 600.0, 1.0),
                       lab::sliderRow(u"Рамки: intensity", model->borderIntensity, 0.0, 6.0, 0.01),
                       lab::sliderRow(u"Рамки: diffuseAmount", model->borderDiffuse, 0.0, 3.0, 0.01),
                       lab::sliderRow(u"Рамки: diffuseAmount у Button3DEffect", model->keyBorderDiffuse, 0.0, 3.0,
                                      0.01),
                       lab::sliderRow(u"Рамки: strokeThickness", model->thickness, 0.5, 8.0, 0.5),
                       lab::noteRow(u"Скругление, пока ползунок не тронут, у каждой клавиши своё: 4 у обычных, 8 у "
                                    u"объёмных."),
                       lab::sliderRow(u"Рамки: CornerRadius", model->corner, 0.0, 24.0, 0.5),
                   }),
        lab::group(u"Оба источника", false,
                   {
                       lab::colorRow(u"color", model->lightColor),
                       lab::sliderRow(u"constantAttenuation", model->constantAttenuation, 0.0, 4.0, 0.01),
                       lab::sliderRow(u"linearAttenuation", model->linearAttenuation, 0.0, 0.05, 0.0005),
                   }),
    });

    return {pad, settings, model};
}
