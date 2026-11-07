// Страница MediaPlayerElement -- MediaPlayerElementPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Content.h>
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.Windows.Storage.Pickers.h>
#include <wxl/Windows.Media.Core.h>
#include <wxl/Windows.Media.Playback.h>
#include <wxl/Windows.Storage.h>
#include "FileTypeChoice.h"

import wxl.core;


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText transportControlsHeader{"Snippets/MediaPlayerElement/MediaplayerelementTransportControls.html"};
gallery::LazyLoadedText transportControlsCode{"Snippets/MediaPlayerElement/MediaplayerelementTransportControls.h"};

FrameworkElement transportControls() {
#include "Snippets/MediaPlayerElement/MediaplayerelementTransportControls.h"

    return gallery::controlExample({
        .header = gallery::snippet(transportControlsHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(transportControlsCode),
    });
}

gallery::LazyLoadedText autoplayHeader{"Snippets/MediaPlayerElement/MediaplayerelementAutoplaysVideo.html"};
gallery::LazyLoadedText autoplayCode{"Snippets/MediaPlayerElement/MediaplayerelementAutoplaysVideo.h"};

FrameworkElement autoplay() {
#include "Snippets/MediaPlayerElement/MediaplayerelementAutoplaysVideo.h"

    return gallery::controlExample({
        .header = gallery::snippet(autoplayHeader),
        .example = example,
        .code = gallery::snippet(autoplayCode),
    });
}

}  // namespace

static wxl::FrameworkElement mediaPlayerElementPage() {
    return StackPanel {transportControls(), autoplay()};
}

REGISTER_GALLERY_PAGE(L"MediaPlayerElement", mediaPlayerElementPage);
