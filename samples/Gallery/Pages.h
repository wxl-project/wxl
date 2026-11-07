#pragma once

// Страницы Gallery: по контролу на файл, как в оригинале (Samples/<Контрол>/).
//
// Страница оригинала — `Page` в `Frame`; здесь она `FrameworkElement`, который
// окно кладёт в `NavigationView::content`, — наименьшее, что умеет
// раскладываться и что принимает содержимое. Раскладку одного примера
// (`ControlExample` оригинала) пишет `controlExample` из Presenter.cpp.
//
// Код каждого примера — файл `Snippets/<Контрол>/X.h`: он включается #include
// в функцию, строящую пример, и он же, прочитанный рядом с exe при первом
// обращении (LazyLoadedText), показывается под примером. Показанное и
// работающее — один текст. Введение к примеру — `X.html` рядом.

#include "Catalog.h"
#include "pch.h"

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gallery {

// Байты сниппета, прочитанные с диска при первом обращении (LazyLoadedText).
using Snippet = std::u8string_view;

// Сниппет, который читается с диска при первом обращении. Держит
// относительный путь (узкий ASCII,
// напр. "Snippets/RadialGradientBrush/RadialGradientBrushSample.html") как
// string_view на строковый литерал — копии нет, литерал живёт всё время — и
// прочитанные байты; load_or_get() читает один раз и отдаёт std::u8string_view над
// ними. Пустое bytes_ значит «ещё не читано»: неудачное чтение бросает и
// приложение останавливается, так что пустых байтов после чтения не бывает.
// UTF-8 не проверяет — это делает snippetText. Чтение синхронное: страница
// строится на STA-потоке, и чтение маленькое. Неудачное чтение бросает
// std::runtime_error с путём, словами системы о коде и самим кодом — той же
// формы, что у xml/json в wxl.core. Некопируемый: держит ссылку на литерал.
class LazyLoadedText : public wxl::core::noncopyable {
public:
    explicit LazyLoadedText(std::string_view relative);

    // Байты файла: при первом вызове читает файл с диска и держит его до
    // конца жизни; неудачное чтение бросает std::runtime_error.
    Snippet load_or_get();

private:
    std::string_view relative_; // путь относительно папки exe (на литерал)
    std::string bytes_;         // прочитанные байты (пустая — ещё не читано)
};

// Тот же snippet(), что зовут страницы, но из LazyLoadedText: страница зовёт
// snippet(sampleHeader) и не знает, откуда байты. Ссылка не const —
// load_or_get() при первом вызове читает файл в байты.
Snippet snippet(LazyLoadedText& lazy);

// Что кладёт в раскладку один пример.
struct ExampleParts {
    Snippet header;                      // введение, HTML
    wxl::FrameworkElement example;       // живой показ
    std::vector<wxl::FrameworkElement> output = {};   // подпись под показом (Output оригинала)
    std::vector<wxl::FrameworkElement> options = {};  // элементы управления справа (Options оригинала)
    Snippet code;                        // исходник показанного
};

// Место страницы: ширина, которую оболочка отдала странице, и ступени, по которым оригинал меняет раскладку. Оригинал
// меряет окно (AdaptiveTrigger); здесь меряется место самой страницы (wxl::AvailableSizeLayout в оболочке): рядом с
// раскрытой панелью навигации его меньше, чем у окна. Поля живут, пока страница на экране.
struct PageSize {
    wxl::core::observable<double> width;
    wxl::core::observable<bool> wide;           // от 641 (Breakpoint640Plus оригинала): широкие поля страницы
    wxl::core::observable<bool> optionsBeside;  // от 740: параметры примера стоят сбоку от него, а не под ним

    PageSize();
};

// Место страницы, которая сейчас строится или показана.
PageSize& pageSize();

// Один ControlExample: введение, показ с параметрами и исходник под ним.
wxl::FrameworkElement controlExample(ExampleParts const& parts);

// Текст сниппета: из UTF-8 в UTF-16, без BOM, без CR и без пустых строк по краям. Общий для Presenter (введение и
// исходник) и prose (проза страницы).
std::wstring snippetText(Snippet bytes);

// Абзац прозы страницы из .html рядом со сниппетами: тот же HtmlBlock, что Presenter ставит введением к примеру, плюс
// один обработчик ссылок — item:<id> ведёт на страницу контрола, http/https запускает адрес, иная схема бросает;
// восстановления парсера тоже бросают (onError), а не молчат. Обработчик ссылок установлен всегда, поэтому BlockedLink
// — клик, погашенный политикой по умолчанию, когда обработчика ссылок нет, — здесь не бывает.
// Отступ — как у RichTextBlock/TextBlock оригинала на этом месте страницы (обычно {0}, у некоторых страниц сверху).
// Выбор текста — по умолчанию как у RichTextBlock (true); контент InfoBar передаёт false, как в оригинале.
wxl::FrameworkElement prose(Snippet html, wxl::Thickness margin, bool textSelection = true);

// Рост под указателем для кнопок самого приложения (заголовок страницы,
// настройки, копирование кода), а не примеров: как у кнопок панели в примере
// Effects.
inline wxl::MagnifyEffect appButtonHoverEffect() {
    return wxl::MagnifyEffect {1.2, wxl::dsl::maximum = 1.35, wxl::dsl::minimum = 0.95};
}

// Страница-образец окон примеров Windowing (SamplePage1..4 оригинала): плитки
// и абзац текста; номер — от 1 до 6.
wxl::FrameworkElement samplePage(int number);

// SampleSettingsPage оригинала: заголовок посреди страницы.
wxl::FrameworkElement sampleSettingsPage();

// Выбор цвета: образец на кнопке и ColorPicker в её выпадающей части
// (ColorSelector оригинала). Цвет — поле модели.
wxl::FrameworkElement colorSelector(wxl::core::observable<wxl::Color>& color, std::u16string_view name);

// Функция, строящая страницу примеров одного контрола.
using ControlPage = wxl::FrameworkElement (*)();

// Страница контрола по идентификатору из каталога или nullptr, если контрол
// ещё не зарегистрирован (в оригинале это IncludedInBuild). Реализация — в
// PageRegistry.cpp.
ControlPage pageFor(std::wstring_view uniqueId);

// Реестр страниц контролов: каждая страница регистрирует себя макросом
// REGISTER_GALLERY_PAGE в своём файле; конструктор ставит запись в общий
// односвязный список, голова которого живёт в PageRegistry.cpp. noncopyable:
// запись держится в списке по адресу, копия или перемещение разорвали бы его.
class PageRegistry : wxl::core::noncopyable {
public:
    PageRegistry(std::wstring_view pageId, ControlPage factory);

private:
    friend ControlPage pageFor(std::wstring_view uniqueId);

    std::wstring_view pageId_;
    ControlPage factory_;
    PageRegistry* next_;
};

// Соединяет два токена в одно имя: две ступени, потому что у операнда `##`
// макрос не разворачивается, и __LINE__ должен стать числом до склейки.
#define GALLERY_CONCAT_(a, b) a##b
#define GALLERY_CONCAT(a, b) GALLERY_CONCAT_(a, b)

// Регистрирует страницу контрола в её файле. Имя переменной — по __LINE__,
// чтобы несколько страниц в одном файле не спорили за имя.
#define REGISTER_GALLERY_PAGE(pageId, factory) \
    static inline const ::gallery::PageRegistry GALLERY_CONCAT(gallery_reg_, __LINE__)(pageId, factory)

// Страница контрола, для которой регистрации пока нет: pageFor её не находит,
// но объявление оставлено, чтобы функция была на виду, когда регистрация
// появится.
wxl::FrameworkElement mapControlPage();

// PagerControl не регистрируется: уже пустой `PagerControl {}` роняет XAML
// (0xC000027B) на установленном рантайме 2.5.1. Причина не установлена;
// контрол числится экспериментальным. Страница написана (PagerControlPage.cpp)
// и собирается; регистрация включается, когда экземпляр заработает.
wxl::FrameworkElement pagerControlPage();

// Плитка контрола (ControlItemTemplate оригинала) и сетка плиток: GridView,
// клик по плитке — переход на страницу контрола.
wxl::FrameworkElement controlTile(ControlInfo const& item);
wxl::GridView tileGrid(std::span<ControlInfo const* const> items, wxl::Thickness padding, wxl::zstring_view id, wxl::zstring_view name);

// Текст как разметка: `&`, `<` и `>` заменены сущностями.
std::wstring htmlEscape(std::wstring_view text);

// Строка из WinRT (char16_t) как wchar_t, каким пользуется каталог.
inline std::wstring wide(std::u16string_view text) {
    return std::wstring(text.begin(), text.end());
}

// Каталог для примеров с поиском: все слова запроса есть в тексте (без учёта
// регистра); названия зарегистрированных контролов, подходящие под запрос;
// контрол по названию; путь к картинке из каталога (`ms-appx:///Assets/…`).
bool containsWords(std::u16string_view text, std::u16string_view query);
std::vector<std::u16string> controlTitles(std::u16string_view query);
ControlInfo const* controlByTitle(std::u16string_view title);
std::wstring assetPath(std::wstring_view imagePath);

// XML с отступами по вложенности: элемент с одним текстом остаётся в строке.
std::wstring indentXml(std::wstring_view xml);

// Служебные страницы оригинала (Pages/).
wxl::FrameworkElement homePage();
wxl::FrameworkElement allControlsPage();
wxl::FrameworkElement sectionPage(ControlGroup const& group);
wxl::FrameworkElement itemPage(ControlInfo const& item);
wxl::FrameworkElement searchResultsPage(std::wstring_view query);
wxl::FrameworkElement settingsPage();

}  // namespace gallery
