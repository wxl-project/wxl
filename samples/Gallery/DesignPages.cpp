// Страницы Design — TypographyPage, GeometryPage, SpacingPage оригинала: введение, картинка с
// подсказками, таблица значений.

#include "Pages.h"

#include "Design.h"

#include <wxl/Microsoft.UI.Xaml.Controls.h>

using namespace wxl;
using namespace wxl::dsl;

namespace {

constexpr char8_t typographyIntroText[] = {
#include "Snippets/Typography/Intro.html.embed"
};
constexpr char8_t geometryIntroText[] = {
#include "Snippets/Geometry/Intro.html.embed"
};
constexpr char8_t spacingIntroText[] = {
#include "Snippets/Spacing/Intro.html.embed"
};

constexpr char8_t typographyHeader[] = {
#include "Snippets/Typography/TypographyTypeRamp.html.embed"
};
constexpr char8_t typographyCode[] = {
#include "Snippets/Typography/TypographyTypeRamp.h.embed"
};
constexpr char8_t geometryHeader[] = {
#include "Snippets/Geometry/Geometry.html.embed"
};
constexpr char8_t geometryCode[] = {
#include "Snippets/Geometry/Geometry.h.embed"
};

FrameworkElement typographyExample() {
#include "Snippets/Typography/TypographyTypeRamp.h"

    auto const heading = Grid {
        hAlign.stretch,
        Margin {0, 48, 0, 24},
        ColumnDefinitions<u"272,136,112,164">,
        gallery::designColumnTitle(u"Example", 0, 16),
        gallery::designColumnTitle(u"Variable Font", 1),
        gallery::designColumnTitle(u"Size/Line height", 2),
        gallery::designColumnTitle(u"Style", 3),
    };
    auto const pictured = StackPanel {
        hAlign.stretch,
        gallery::pinnedImage(
            u"Assets/Design/Typography", 750, 450,
            {{650, 60, u"Caption", u"", u"Caption"},
             {190, 280, u"Body", u"", u"Body"},
             {83, 245, u"Body Strong", u"", u"Body Strong"},
             {320, 20, u"Title", u"", u"Title"},
             {160, 110, u"Display"}}),
        ScrollViewer {
            horizontalScrollBarVisibility = ScrollBarVisibility::Auto,
            horizontalScrollMode = ScrollMode::Auto,
            verticalScrollBarVisibility = ScrollBarVisibility::Hidden,
            content = StackPanel {heading, example},
        },
    };
    return gallery::controlExample({
        .header = gallery::snippet(typographyHeader),
        .example = pictured,
        .code = gallery::snippet(typographyCode),
    });
}

FrameworkElement geometryExample() {
#include "Snippets/Geometry/Geometry.h"

    auto const heading = Grid {
        hAlign.stretch,
        Margin {0, 48, 0, 24},
        ColumnDefinitions<u"148,*,112">,
        gallery::designColumnTitle(u"Corner radius", 0, 16),
        gallery::designColumnTitle(u"Usage", 1),
        gallery::designColumnTitle(u"Style", 2),
    };
    auto const pictured = StackPanel {
        hAlign.stretch,
        gallery::pinnedImage(u"Assets/Design/Geometry", 505, 271,
                             {{16, 16, u"8px", u"OverlayCornerRadius", u"8px"},
                              {16, 148, u"0px", u"", u"Body"},
                              {240, 168, u"4px", u"ControlCornerRadius", u"4px"}}),
        ScrollViewer {
            horizontalScrollBarVisibility = ScrollBarVisibility::Auto,
            horizontalScrollMode = ScrollMode::Auto,
            verticalScrollBarVisibility = ScrollBarVisibility::Hidden,
            content = StackPanel {heading, example},
        },
    };
    return gallery::controlExample({
        .header = gallery::snippet(geometryHeader),
        .example = pictured,
        .code = gallery::snippet(geometryCode),
    });
}

Grid spacingRow(char16_t const* value, double size, char16_t const* usage, bool shaded, double top = 0) {
    auto row = gallery::designRow(shaded, Thickness {0, top, 0, 0});
    row.columnDefinitions(ColumnDefinitions<u"86,136,*">);
    row.children().append(TextBlock {Margin {16, 0, 0, 0}, vAlign.center, value});
    row.children().append(Border {column = 1, width = size, height = 20, hAlign.left, cornerRadius = CornerRadius {4},
                                  background = brushes.Accent.FillColor.Default});
    row.children().append(TextBlock {column = 2, vAlign.center, textWrapping = TextWrapping::Wrap, styles.TextBlock.Caption, usage});
    return row;
}

FrameworkElement spacingExample(char16_t const* title, char16_t const* stem, double height) {
    return Grid {
        vAlign.top,
        rowSpacing = 12.0,
        RowDefinitions<u"auto,*">,
        TextBlock {hAlign.center, styles.TextBlock.Subtitle, title},
        Border {row = 1, gallery::themedImage(stem, height)},
    };
}

}  // namespace

static FrameworkElement typographyPage() {
    return StackPanel {
        gallery::prose(gallery::snippet(typographyIntroText), Thickness {0, 4, 0, 0}),
        typographyExample(),
    };
}

static FrameworkElement geometryPage() {
    return StackPanel {
        gallery::prose(gallery::snippet(geometryIntroText), Thickness {}),
        geometryExample(),
    };
}

static FrameworkElement spacingPage() {
    return StackPanel {
        gallery::prose(gallery::snippet(spacingIntroText), Thickness {}),
        ScrollViewer {
            Margin {0, 24, 0, 0},
            horizontalScrollBarVisibility = ScrollBarVisibility::Auto,
            horizontalScrollMode = ScrollMode::Auto,
            content = StackPanel {
                Margin {0, 0, 0, 16},
                orientation.horizontal,
                spacing = 36.0,
                spacingExample(u"Page with cards layout", u"Assets/Design/Cards", 400),
                spacingExample(u"Form layout", u"Assets/Design/Dialog", 400),
            },
        },
        Border {
            Margin {0, 36, 0, 0},
            background = brushes.Card.BackgroundFillColor.Default,
            borderBrush = brushes.Card.StrokeColorDefault,
            BorderThickness {1},
            cornerRadius = CornerRadius {8},
            child = ScrollViewer {
                horizontalScrollBarVisibility = ScrollBarVisibility::Auto,
                horizontalScrollMode = ScrollMode::Auto,
                verticalScrollBarVisibility = ScrollBarVisibility::Hidden,
                content = StackPanel {
                    Padding {12, 24, 12, 12},
                    Grid {
                        Margin {16, 0, 0, 0},
                        hAlign.stretch,
                        ColumnDefinitions<u"206,*">,
                        gallery::designColumnTitle(u"Value"),
                        gallery::designColumnTitle(u"Usage", 1),
                    },
                    spacingRow(u"4epx", 4, u"Spacing used for compact sizing.", true, 24),
                    spacingRow(u"8epx", 8, u"Spacing between UI controls, control + label.", false),
                    spacingRow(u"12epx", 12, u"Spacing between control + header, surface and edge text, text sections.", true),
                    spacingRow(u"16epx", 16, u"Padding used in list styles, cards.", false),
                    spacingRow(u"24epx", 24, u"Spacing between content sections.", true),
                    spacingRow(u"36epx", 36, u"Padding on pages.", false),
                    spacingRow(u"48epx", 48, u"Spacing between page sections with title.", true),
                },
            },
        },
    };
}

REGISTER_GALLERY_PAGE(L"Typography", typographyPage);
REGISTER_GALLERY_PAGE(L"Geometry", geometryPage);
REGISTER_GALLERY_PAGE(L"Spacing", spacingPage);
