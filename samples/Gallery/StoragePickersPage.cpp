// Страница StoragePickers -- StoragePickersPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include <cmath>
#include <wxl/Windows.Storage.h>
#include <wxl/Windows.Storage.FileProperties.h>
#include <wxl/Microsoft.Windows.Storage.Pickers.h>
#include "FileTypeChoice.h"
#include <wxl/Microsoft.UI.Content.h>

#include <wxl/Microsoft.UI.Xaml.Media.Imaging.h>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText singleHeader{"Snippets/StoragePickers/StoragePickersPickSingleFile.html"};
gallery::LazyLoadedText singleCode{"Snippets/StoragePickers/StoragePickersPickSingleFile.h"};

FrameworkElement single() {
#include "Snippets/StoragePickers/StoragePickersPickSingleFile.h"

    return gallery::controlExample({
        .header = gallery::snippet(singleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(singleCode),
    });
}

gallery::LazyLoadedText multipleHeader{"Snippets/StoragePickers/StoragePickersPickMultipleFiles.html"};
gallery::LazyLoadedText multipleCode{"Snippets/StoragePickers/StoragePickersPickMultipleFiles.h"};

FrameworkElement multiple() {
#include "Snippets/StoragePickers/StoragePickersPickMultipleFiles.h"

    return gallery::controlExample({
        .header = gallery::snippet(multipleHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(multipleCode),
    });
}

gallery::LazyLoadedText saveHeader{"Snippets/StoragePickers/StoragePickersSaveFile.html"};
gallery::LazyLoadedText saveCode{"Snippets/StoragePickers/StoragePickersSaveFile.h"};

FrameworkElement save() {
#include "Snippets/StoragePickers/StoragePickersSaveFile.h"

    return gallery::controlExample({
        .header = gallery::snippet(saveHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(saveCode),
    });
}

gallery::LazyLoadedText folderHeader{"Snippets/StoragePickers/StoragePickersPickFolder.html"};
gallery::LazyLoadedText folderCode{"Snippets/StoragePickers/StoragePickersPickFolder.h"};

FrameworkElement folder() {
#include "Snippets/StoragePickers/StoragePickersPickFolder.h"

    return gallery::controlExample({
        .header = gallery::snippet(folderHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(folderCode),
    });
}

gallery::LazyLoadedText thumbnailHeader{"Snippets/StoragePickers/StoragePickersFileThumbnail.html"};
gallery::LazyLoadedText thumbnailCode{"Snippets/StoragePickers/StoragePickersFileThumbnail.h"};

FrameworkElement thumbnail() {
#include "Snippets/StoragePickers/StoragePickersFileThumbnail.h"

    return gallery::controlExample({
        .header = gallery::snippet(thumbnailHeader),
        .example = example,
        .options = {options},
        .code = gallery::snippet(thumbnailCode),
    });
}

}  // namespace

static wxl::FrameworkElement storagePickersPage() {
    return StackPanel {
        // Подсказка оригинала вверху страницы. Отступ prose {0,12,12,12} — дословно отступ RichTextBlock оригинала: prose
        // кладёт HtmlBlock ровно в Content InfoBar, где стоял RichTextBlock, поэтому отступ переносится дословно.
        InfoBar {
            isClosable = false,
            isOpen = true,
            Margin {0, 8, 0, 0},
            content = gallery::prose(
                u8"The picker reopens in the <span style=\"semibold\">last selected location and view</span>. The "
                u8"<code><span style=\"semibold\">SuggestedStartLocation</span></code> and "
                u8"<code><span style=\"semibold\">ViewMode</span></code> are only applied the "
                u8"<span style=\"semibold\">first time</span> the picker is opened (for example, right after app "
                u8"installation or when no previous selection exists).",
                Thickness {0, 12, 12, 12},
                false),
        },
        single(), multiple(), save(), folder(), thumbnail(),
    };
}

REGISTER_GALLERY_PAGE(L"StoragePickers", storagePickersPage);
