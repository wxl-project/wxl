// Страница Clipboard -- ClipboardPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "Application.h"
#include <wxl/Windows.ApplicationModel.DataTransfer.h>
#include <wxl/Windows.Storage.h>
#include <wxl/Windows.Storage.Streams.h>
#include <wxl/Microsoft.Windows.Storage.Pickers.h>
#include <wxl/Microsoft.UI.Content.h>

#include <wxl/Microsoft.UI.Xaml.Media.Imaging.h>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText copyTextHeader{"Snippets/Clipboard/CopyTextClipboard.html"};
gallery::LazyLoadedText copyTextCode{"Snippets/Clipboard/CopyTextClipboard.h"};

FrameworkElement copyText() {
#include "Snippets/Clipboard/CopyTextClipboard.h"

    return gallery::controlExample({
        .header = gallery::snippet(copyTextHeader),
        .example = example,
        .code = gallery::snippet(copyTextCode),
    });
}

gallery::LazyLoadedText pasteTextHeader{"Snippets/Clipboard/PasteTextClipboard.html"};
gallery::LazyLoadedText pasteTextCode{"Snippets/Clipboard/PasteTextClipboard.h"};

FrameworkElement pasteText() {
#include "Snippets/Clipboard/PasteTextClipboard.h"

    return gallery::controlExample({
        .header = gallery::snippet(pasteTextHeader),
        .example = example,
        .code = gallery::snippet(pasteTextCode),
    });
}

gallery::LazyLoadedText imageHeader{"Snippets/Clipboard/ClipboardCopyPasteImage.html"};
gallery::LazyLoadedText imageCode{"Snippets/Clipboard/ClipboardCopyPasteImage.h"};

FrameworkElement image() {
#include "Snippets/Clipboard/ClipboardCopyPasteImage.h"

    return gallery::controlExample({
        .header = gallery::snippet(imageHeader),
        .example = example,
        .code = gallery::snippet(imageCode),
    });
}

gallery::LazyLoadedText filesHeader{"Snippets/Clipboard/ClipboardCopyPasteFiles.html"};
gallery::LazyLoadedText filesCode{"Snippets/Clipboard/ClipboardCopyPasteFiles.h"};

FrameworkElement files() {
#include "Snippets/Clipboard/ClipboardCopyPasteFiles.h"

    return gallery::controlExample({
        .header = gallery::snippet(filesHeader),
        .example = example,
        .code = gallery::snippet(filesCode),
    });
}

gallery::LazyLoadedText optionsHeader{"Snippets/Clipboard/ClipboardHistoryRoamingOptions.html"};
gallery::LazyLoadedText optionsCode{"Snippets/Clipboard/ClipboardHistoryRoamingOptions.h"};

FrameworkElement options() {
#include "Snippets/Clipboard/ClipboardHistoryRoamingOptions.h"

    return gallery::controlExample({
        .header = gallery::snippet(optionsHeader),
        .example = example,
        .code = gallery::snippet(optionsCode),
    });
}

gallery::LazyLoadedText otherHeader{"Snippets/Clipboard/OtherClipboardOperations.html"};
gallery::LazyLoadedText otherCode{"Snippets/Clipboard/OtherClipboardOperations.h"};

FrameworkElement other() {
#include "Snippets/Clipboard/OtherClipboardOperations.h"

    return gallery::controlExample({
        .header = gallery::snippet(otherHeader),
        .example = example,
        .code = gallery::snippet(otherCode),
    });
}

}  // namespace

static wxl::FrameworkElement clipboardPage() {
    return StackPanel {copyText(), pasteText(), image(), files(), options(), other()};
}

REGISTER_GALLERY_PAGE(L"Clipboard", clipboardPage);
