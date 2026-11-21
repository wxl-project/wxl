// Страница DatePicker -- DatePickerPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText withHeaderHeader{"Snippets/DatePicker/Header.html"};
gallery::LazyLoadedText withHeaderCode{"Snippets/DatePicker/Header.h"};

FrameworkElement withHeader() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(withHeaderHeader),
        .example =
#include "Snippets/DatePicker/Header.h"
        ,
        .code = gallery::snippet(withHeaderCode),
    });
}

gallery::LazyLoadedText dayFormattedYearHeader{"Snippets/DatePicker/DayFormattedYear.html"};
gallery::LazyLoadedText dayFormattedYearCode{"Snippets/DatePicker/DayFormattedYear.h"};

FrameworkElement dayFormattedYear() {
    using namespace wxl::dsl;
#include "Snippets/DatePicker/DayFormattedYear.h"

    return gallery::controlExample({
        .header = gallery::snippet(dayFormattedYearHeader),
        .example = picker,
        .code = gallery::snippet(dayFormattedYearCode),
    });
}

}  // namespace

static wxl::FrameworkElement datePickerPage() {
    return StackPanel {withHeader(), dayFormattedYear()};
}

REGISTER_GALLERY_PAGE(L"DatePicker", datePickerPage);
