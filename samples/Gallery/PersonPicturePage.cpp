// Страница PersonPicture -- PersonPicturePage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <wxl/Microsoft.UI.Xaml.Controls.h>
#include <wxl/Microsoft.UI.Xaml.Media.Imaging.h>


using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText looksHeader{"Snippets/PersonPicture/PersonPictureSelectDifferentLooksPerson.html"};
gallery::LazyLoadedText looksCode{"Snippets/PersonPicture/PersonPictureSelectDifferentLooksPerson.h"};

FrameworkElement looks() {
#include "Snippets/PersonPicture/PersonPictureSelectDifferentLooksPerson.h"

    return gallery::controlExample({
        .header = gallery::snippet(looksHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(looksCode),
    });
}

}  // namespace

static wxl::FrameworkElement personPicturePage() {
    return StackPanel {looks()};
}

REGISTER_GALLERY_PAGE(L"PersonPicture", personPicturePage);
