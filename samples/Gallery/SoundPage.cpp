// Страница Sound -- SoundPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <string>
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.h>
#include <wxl/Microsoft.UI.Xaml.Documents.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText togglingHeader{"Snippets/Sound/TogglingSound.html"};
gallery::LazyLoadedText togglingCode{"Snippets/Sound/TogglingSound.h"};

FrameworkElement toggling() {
#include "Snippets/Sound/TogglingSound.h"

    return gallery::controlExample({
        .header = gallery::snippet(togglingHeader),
        .example = example,
        .code = gallery::snippet(togglingCode),
    });
}

gallery::LazyLoadedText spatialHeader{"Snippets/Sound/SoundTogglingSpatialAudio.html"};
gallery::LazyLoadedText spatialCode{"Snippets/Sound/SoundTogglingSpatialAudio.h"};

FrameworkElement spatial() {
#include "Snippets/Sound/SoundTogglingSpatialAudio.h"

    return gallery::controlExample({
        .header = gallery::snippet(spatialHeader),
        .example = example,
        .code = gallery::snippet(spatialCode),
    });
}

gallery::LazyLoadedText systemSoundHeader{"Snippets/Sound/PlaySpecificSystemSound.html"};
gallery::LazyLoadedText systemSoundCode{"Snippets/Sound/PlaySpecificSystemSound.h"};

FrameworkElement systemSound() {
#include "Snippets/Sound/PlaySpecificSystemSound.h"

    return gallery::controlExample({
        .header = gallery::snippet(systemSoundHeader),
        .example = example,
        .code = gallery::snippet(systemSoundCode),
    });
}

}  // namespace

static wxl::FrameworkElement soundPage() {
    return StackPanel {toggling(), spatial(), systemSound()};
}

REGISTER_GALLERY_PAGE(L"Sound", soundPage);
