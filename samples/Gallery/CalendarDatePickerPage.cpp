// Страница CalendarDatePicker -- CalendarDatePickerPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;
using namespace wxl::dsl;

namespace {

gallery::LazyLoadedText headerPlaceholderHeader{"Snippets/CalendarDatePicker/HeaderPlaceholder.html"};
gallery::LazyLoadedText headerPlaceholderCode{"Snippets/CalendarDatePicker/HeaderPlaceholder.h"};

FrameworkElement headerPlaceholder() {
    return gallery::controlExample({
        .header = gallery::snippet(headerPlaceholderHeader),
        .example =
#include "Snippets/CalendarDatePicker/HeaderPlaceholder.h"
        ,
        .code = gallery::snippet(headerPlaceholderCode),
    });
}

}  // namespace

static wxl::FrameworkElement calendarDatePickerPage() {
    return StackPanel {headerPlaceholder()};
}

REGISTER_GALLERY_PAGE(L"CalendarDatePicker", calendarDatePickerPage);
