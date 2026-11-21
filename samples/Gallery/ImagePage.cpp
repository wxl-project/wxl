// Страница Image -- ImagePage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.Imaging.h>


using namespace wxl;

namespace {

gallery::LazyLoadedText basicHeader{"Snippets/Image/BasicImageLocalFile.html"};
gallery::LazyLoadedText basicCode{"Snippets/Image/BasicImageLocalFile.h"};

FrameworkElement basic() {
    using namespace wxl::dsl;
#include "Snippets/Image/BasicImageLocalFile.h"

    return gallery::controlExample({
        .header = gallery::snippet(basicHeader),
        .example = example,
        .code = gallery::snippet(basicCode),
    });
}

gallery::LazyLoadedText decodedHeader{"Snippets/Image/ImageDecodedRenderingSize.html"};
gallery::LazyLoadedText decodedCode{"Snippets/Image/ImageDecodedRenderingSize.h"};

FrameworkElement decoded() {
    using namespace wxl::dsl;
#include "Snippets/Image/ImageDecodedRenderingSize.h"

    return gallery::controlExample({
        .header = gallery::snippet(decodedHeader),
        .example = example,
        .code = gallery::snippet(decodedCode),
    });
}

gallery::LazyLoadedText stretchingHeader{"Snippets/Image/ImageStretching.html"};
gallery::LazyLoadedText stretchingCode{"Snippets/Image/ImageStretching.h"};

FrameworkElement stretching() {
    using namespace wxl::dsl;
#include "Snippets/Image/ImageStretching.h"

    return gallery::controlExample({
        .header = gallery::snippet(stretchingHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(stretchingCode),
    });
}

gallery::LazyLoadedText nineGridImagesHeader{"Snippets/Image/NineGridImages.html"};
gallery::LazyLoadedText nineGridImagesCode{"Snippets/Image/NineGridImages.h"};

FrameworkElement nineGridImages() {
    using namespace wxl::dsl;
#include "Snippets/Image/NineGridImages.h"

    return gallery::controlExample({
        .header = gallery::snippet(nineGridImagesHeader),
        .example = example,
        .code = gallery::snippet(nineGridImagesCode),
    });
}

gallery::LazyLoadedText svgHeader{"Snippets/Image/SvgImage.html"};
gallery::LazyLoadedText svgCode{"Snippets/Image/SvgImage.h"};

FrameworkElement svg() {
    using namespace wxl::dsl;
#include "Snippets/Image/SvgImage.h"

    return gallery::controlExample({
        .header = gallery::snippet(svgHeader),
        .example = example,
        .code = gallery::snippet(svgCode),
    });
}

gallery::LazyLoadedText animatedGifHeader{"Snippets/Image/AnimatedGif.html"};
gallery::LazyLoadedText animatedGifCode{"Snippets/Image/AnimatedGif.h"};

FrameworkElement animatedGif() {
    using namespace wxl::dsl;
#include "Snippets/Image/AnimatedGif.h"

    return gallery::controlExample({
        .header = gallery::snippet(animatedGifHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(animatedGifCode),
    });
}

}  // namespace

static wxl::FrameworkElement imagePage() {
    return StackPanel {basic(), decoded(), stretching(), nineGridImages(), svg(), animatedGif()};
}

REGISTER_GALLERY_PAGE(L"Image", imagePage);
