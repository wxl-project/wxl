// Страница TimePicker -- TimePickerPage оригинала.

#include "Pages.h"
#include "Shell.h"

using namespace wxl;

namespace {

gallery::LazyLoadedText simpleHeader{"Snippets/TimePicker/Simple.html"};
gallery::LazyLoadedText simpleCode{"Snippets/TimePicker/Simple.h"};

FrameworkElement simple() {
    return gallery::controlExample({
        .header = gallery::snippet(simpleHeader),
        .example =
#include "Snippets/TimePicker/Simple.h"
        ,
        .code = gallery::snippet(simpleCode),
    });
}

gallery::LazyLoadedText minuteIncrementsHeader{"Snippets/TimePicker/HeaderMinuteIncrements.html"};
gallery::LazyLoadedText minuteIncrementsCode{"Snippets/TimePicker/HeaderMinuteIncrements.h"};

FrameworkElement minuteIncrements() {
    using namespace wxl::dsl;
    return gallery::controlExample({
        .header = gallery::snippet(minuteIncrementsHeader),
        .example =
#include "Snippets/TimePicker/HeaderMinuteIncrements.h"
        ,
        .code = gallery::snippet(minuteIncrementsCode),
    });
}

gallery::LazyLoadedText clockIdentifiersHeader{"Snippets/TimePicker/ClockIdentifiers.html"};
gallery::LazyLoadedText clockIdentifiersCode{"Snippets/TimePicker/ClockIdentifiers.h"};

FrameworkElement clockIdentifiers() {
    using namespace wxl::dsl;
#include "Snippets/TimePicker/ClockIdentifiers.h"

    return gallery::controlExample({
        .header = gallery::snippet(clockIdentifiersHeader),
        .example = pickers,
        .code = gallery::snippet(clockIdentifiersCode),
    });
}

}  // namespace

static wxl::FrameworkElement timePickerPage() {
    return StackPanel {simple(), minuteIncrements(), clockIdentifiers()};
}

REGISTER_GALLERY_PAGE(L"TimePicker", timePickerPage);
