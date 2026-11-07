// Страница JumpList -- JumpListPage оригинала.

#include "Pages.h"
#include "Shell.h"
#include "Application.h"
#include <wxl/Windows.UI.StartScreen.h>

import wxl.core;

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText tasksHeader{"Snippets/JumpList/AddingTasksJumpList.html"};
gallery::LazyLoadedText tasksCode{"Snippets/JumpList/AddingTasksJumpList.h"};

FrameworkElement tasks() {
#include "Snippets/JumpList/AddingTasksJumpList.h"

    return gallery::controlExample({
        .header = gallery::snippet(tasksHeader),
        .example = example,
        .code = gallery::snippet(tasksCode),
    });
}

gallery::LazyLoadedText groupHeader{"Snippets/JumpList/JumpListAddingItemsCustomGroup.html"};
gallery::LazyLoadedText groupCode{"Snippets/JumpList/JumpListAddingItemsCustomGroup.h"};

FrameworkElement group() {
#include "Snippets/JumpList/JumpListAddingItemsCustomGroup.h"

    return gallery::controlExample({
        .header = gallery::snippet(groupHeader),
        .example = example,
        .code = gallery::snippet(groupCode),
    });
}

}  // namespace

static wxl::FrameworkElement jumpListPage() {
    // What the original says above its examples: the list belongs to a packaged application.
    return StackPanel {
        InfoBar {Margin {0, 10, 0, 0}, isOpen = true, isClosable = false, severity = InfoBarSeverity::Warning,
                 title = u"JumpList is not available in unpackaged mode.",
                 message = u"This API requires the app to be running in packaged mode (MSIX)."},
        TextBlock {Margin {0, 12, 0, 0}, textWrapping = TextWrapping::Wrap,
                   u"WinUI Gallery populates its jump list with your recently visited and favorited items. Restarting the app will "
                   u"restore these entries after any changes made on this page."},
        tasks(),
        group(),
    };
}

REGISTER_GALLERY_PAGE(L"JumpList", jumpListPage);
