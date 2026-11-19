// Страница AnimatedVisualPlayer -- AnimatedVisualPlayerPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "LottieLogo.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText lottieHeader{"Snippets/AnimatedVisualPlayer/AnimatedVisualPlayerPlaybackLottieAnimation.html"};
gallery::LazyLoadedText lottieCode{"Snippets/AnimatedVisualPlayer/AnimatedVisualPlayerPlaybackLottieAnimation.h"};

FrameworkElement lottie() {
#include "Snippets/AnimatedVisualPlayer/AnimatedVisualPlayerPlaybackLottieAnimation.h"

    return gallery::controlExample({
        .header = gallery::snippet(lottieHeader),
        .example = example,
        .code = gallery::snippet(lottieCode),
    });
}

}  // namespace

static wxl::FrameworkElement animatedVisualPlayerPage() {
    return StackPanel {lottie()};
}

REGISTER_GALLERY_PAGE(L"AnimatedVisualPlayer", animatedVisualPlayerPage);
