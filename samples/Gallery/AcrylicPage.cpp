// Страница Acrylic -- AcrylicPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText introText{"Snippets/Acrylic/Intro.html"};

gallery::LazyLoadedText defaultBrushHeader{"Snippets/Acrylic/DefaultAppAcrylicBrush.html"};
gallery::LazyLoadedText defaultBrushCode{"Snippets/Acrylic/DefaultAppAcrylicBrush.h"};

FrameworkElement defaultBrush() {
#include "Snippets/Acrylic/DefaultAppAcrylicBrush.h"

    return gallery::controlExample({
        .header = gallery::snippet(defaultBrushHeader),
        .example = example,
        .code = gallery::snippet(defaultBrushCode),
    });
}

gallery::LazyLoadedText customBrushHeader{"Snippets/Acrylic/CustomAcrylicAppBrush.html"};
gallery::LazyLoadedText customBrushCode{"Snippets/Acrylic/CustomAcrylicAppBrush.h"};

FrameworkElement customBrush() {
#include "Snippets/Acrylic/CustomAcrylicAppBrush.h"

    return gallery::controlExample({
        .header = gallery::snippet(customBrushHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(customBrushCode),
    });
}

gallery::LazyLoadedText luminosityHeader{"Snippets/Acrylic/LuminosityAppAcrylic.html"};
gallery::LazyLoadedText luminosityCode{"Snippets/Acrylic/LuminosityAppAcrylic.h"};

FrameworkElement luminosity() {
#include "Snippets/Acrylic/LuminosityAppAcrylic.h"

    return gallery::controlExample({
        .header = gallery::snippet(luminosityHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(luminosityCode),
    });
}

}  // namespace

static wxl::FrameworkElement acrylicPage() {
    return StackPanel {
        gallery::prose(gallery::snippet(introText), Thickness {}), defaultBrush(), customBrush(), luminosity()};
}

REGISTER_GALLERY_PAGE(L"Acrylic", acrylicPage);
