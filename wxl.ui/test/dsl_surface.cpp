// Compile-time check of the declarative surface over full's vocabulary.
//
// Part of wxl.ui.full.surface-test only: many of the classes written here
// only full generates. The surface tests of the other profiles compile their
// generated schema_surface.cpp and the fragments of the components they
// select, without this file.
//
// It is built as its own target *without* the cppwinrt include path, so it
// proves two things at once: that the builder syntax below compiles against
// the generated wrappers, and that reaching that syntax needs no winrt
// header at all -- if a winrt:: name ever leaked into a public header, this
// translation unit would stop compiling rather than quietly get slower.
//
// Nothing here runs: the WinUI runtime is not up in a test, and activating
// a real control without it fails. This is about the syntax and the types.
//
// The same target compiles wxl/schema_surface.cpp: one line per
// element of the generated schema.h, and nothing else. What it proves is
// that every anchor the schema offers actually applies to the class it hangs
// on -- that the key reaches a setter that class has, that the value type the
// schema names is one that setter takes, and that the owner check passes for
// the class itself. It is generated from the same closure as the schema,
// which is what makes the coverage rule enforceable: a new type or a new
// member appears in both files or in neither.
//
// The one thing it cannot check is the negative: that the owner check
// *refuses* another class's member. The check is a static_assert inside a
// function body, and a body is not instantiated by a requires-expression, so
// there is no way to assert that it fails.
//
// What a component of the profile layer brings is checked beside its sources,
// in the surface fragment its manifest names (key "surface"): the profile's
// surface test compiles the fragments of the components it selects. This file
// keeps the rest: the generated classes and what no component owns yet.

#include "Bind.h"
#include "event_awaitable.h"
#include "method.h"
#include "Relief.h"
#include "UiThread.h"
#include "WaitableTimer.h"
#include "ui.h"
#include <wxl/Microsoft.UI.Dispatching.h>
#include "schema.h"
#include <wxl/Microsoft.UI.Composition.h>
#include <wxl/Microsoft.UI.Xaml.Hosting.h>
#include <wxl/brushes.h>

namespace {

using namespace wxl;

// The vocabulary of the syntax lives in a namespace of its own and is brought
// in deliberately, so that naming a wxl type does not drag every property
// name into scope with it. The two never collide: a type is PascalCase and a
// member is camelCase, so `Margin` is the type and `margin` the tag.
using namespace wxl::dsl;

// Named arguments, an event handler, and unnamed arguments routed by type:
// a HorizontalAlignment can only mean HorizontalAlignment, and a string on
// a ContentControl can only mean Content.
[[maybe_unused]] void named_and_positional() {
    Button explicit_names{
        content = L"Click",
        horizontalAlignment = HorizontalAlignment::Center,
        onClick += [](Object const&, RoutedEventArgs&) {},
    };

    Button short_form{
        L"Click",
        hAlign.center,
        vAlign.center,
    };

    TextBlock text{
        L"WinUI in C++ Without XAML!",
        hAlign.center,
    };

    StackPanel panel{
        Orientation::Vertical,
        spacing = 8.0,
    };
}

// A collection-valued property is subscripted, not assigned to, and the
// items arrive as a parameter pack -- each keeps its own type instead of
// being sliced to a common base before the collection sees it.
[[maybe_unused]] void children_list() {
    StackPanel panel{
        hAlign.center,
        children[TextBlock{L"WinUI in C++ Without XAML!", hAlign.center},
                 Button{content = L"Click"}],
    };
}

// The same list without the property name: an element written straight
// inside its parent's braces goes to the class's content collection, which
// the metadata names itself.
[[maybe_unused]] void unnamed_children() {
    StackPanel panel{
        hAlign.center,
        orientation.horizontal,
        Image{source = L"Assets/logo.png", hAlign.right},
        TextBlock{L"WinUI in C++ Without XAML!"},
        Button{content = L"Click"},
    };

    // A lone child is still a child as long as it is not of the parent's own
    // kind -- that case belongs to the copy constructor.
    StackPanel single{Button{content = L"Click"}};
}

// A repeated child, and the properties a child carries for its parent's
// layout: the count is part of repeat's type, and `row` reaches
// Grid::setRow(child, value) although it is written inside the child. The
// rows are the same property the collection form subscripts, parsed from text.
[[maybe_unused]] void repeated_children() {
    Grid grid{
        RowDefinitions<u"2*,*,*">,
        ColumnDefinitions<u"*,*">,
        TextBlock{L"header", row = 0, columnSpan = 2},
        [](repeat<4> index) {
            return Button{
                content = L"key",
                row = index / 2 + 1,
                column = index % 2,
            };
        },
    };
}

// The same over given values: a literal's characters, a list written out, a
// constexpr array, or any mixture of those -- one parameter pack takes all
// of them, and the body is handed the element and its position. `text()` is
// there only where the elements are text.
[[maybe_unused]] void iterated_children() {
    StackPanel keypad{
        [](iterate<L"789"> key) {
            return Button{
                key.text(),
                onClick +=
                [key](Object const&, RoutedEventArgs&) {
                    wchar_t const pressed = key;
                    (void)pressed;
                },
            };
        },
    };

    // What counts as an element, checked where it is cheapest to check: a
    // literal's terminator is not one, an array that merely ends in a zero
    // keeps it, and a character array without a terminator keeps all of it.
    static constexpr char unterminated[]{'a', 'b', 'c'};
    static constexpr int ending_in_zero[]{1, 4, 0};
    static_assert(iterate<L"abc">::count == 3);
    static_assert(iterate<L"">::count == 0);
    static_assert(iterate<unterminated>::count == 3);
    static_assert(iterate<ending_in_zero>::count == 3);
    static_assert(iterate<0>::count == 1);
    static_assert(iterate<L"ab", L'c', 'd'>::count == 4);

    // A run of characters is terminated in storage whether its source was
    // or not, so the elements are always a valid string of `count`.
    static constexpr impl::values from_literal = L"abc";
    static constexpr impl::values from_array = unterminated;
    static constexpr impl::values from_value = L'x';
    static_assert(from_literal.count == 3 && from_literal.items[from_literal.count] == L'\0');
    static_assert(from_array.count == 3 && from_array.items[from_array.count] == '\0');
    static_assert(from_value.count == 1 && from_value.items[from_value.count] == L'\0');

    static constexpr int spans[]{1, 2, 4};
    Grid layout{
        ColumnDefinitions<u"*,*,*,*">,
        [](iterate<0, 1, 2> at) { return TextBlock{L"head", column = at}; },
        [](iterate<spans> span) { return TextBlock{L"body", row = 1, columnSpan = span}; },
        [](iterate<L"ab", L'c'> mixed) { return TextBlock{mixed.text(), row = 2}; },
    };
}

// Margin and Padding are wxl types of their own, so they read as braced
// lists and route positionally without the property name.
[[maybe_unused]] void thicknesses() {
    Button uniform{margin = {20}};
    Button pairs{margin = {20, 8}, padding = {4, 2}};
    Button sides{margin = {1, 2, 3, 4}};
    Button positional{Margin{20}, Padding{4}};
}

// A tag over a built-in: thousandths of an em is an int32, and an int says
// nothing about which property it belongs to, so the unnamed form needs the
// tag while the named one stays an ordinary assignment.
[[maybe_unused]] void spacing() {
    TextBlock tagged{L"wide", CharacterSpacing{60}};
    TextBlock named{L"wide", characterSpacing = 60};
}

// A property wxl declares although WinRT has none: the minimum size lives
// on the window's presenter, three hops away, and reads here as one word.
// A static member is a call on the class, not on an object.
[[maybe_unused]] void window_constraints(Window const& window) {
    Window bounded{minSize = {600, 400}};

    OverlappedPresenter const presenter = OverlappedPresenter::create();
    presenter.preferredMinimumWidth(600);
    core::nullable<int32_t> const width = presenter.preferredMinimumWidth();
    presenter.preferredMaximumWidth({});
    window.appWindow().setPresenter(presenter);
    (void)width;
}

// A method written as a tag: Window.SetTitleBar takes one argument and hands
// nothing back, and the profile narrows it to the TitleBar control -- so the
// tag takes one built, builds one from braces, and a lone TitleBar inside the
// window's braces is routed to it by type. The button colours are nullable:
// empty hands the colour back to the system.
[[maybe_unused]] void window_title_bar(Window const& window, TitleBar const& built) {
    Window assigned{extendsContentIntoTitleBar = true, titleBar = built};
    Window braced{
        extendsContentIntoTitleBar = true,
        titleBar = {leftHeader = TextBlock{L"App"}, rightHeader = Button{L"Sign in"}},
    };
    Window routed{built};
    window.titleBar(built);

    AppWindowTitleBar const bar = window.appWindow().titleBar();
    bar.preferredHeightOption(TitleBarHeightOption::Tall);
    bar.buttonBackgroundColor(colors.transparent);
    bar.buttonHoverBackgroundColor({});
    core::nullable<Color> const background = bar.buttonBackgroundColor();
    (void)background;
}

// One reading of a member for every spelling: const or not, noexcept or not.
struct Spellings {
    int plain(char, double);
    void constant(int) const;
    void quiet() noexcept;
    bool both(Object const&, float) const noexcept;
};

template <auto Member, typename Class, typename Result, typename Parameters, bool Const,
          bool Noexcept>
constexpr bool reads_as() {
    using traits = impl::member_function_traits<decltype(Member)>;
    return std::is_same_v<typename traits::class_t, Class> &&
           std::is_same_v<typename traits::result_t, Result> &&
           std::is_same_v<typename traits::parameters_t, Parameters> && traits::is_const == Const &&
           traits::is_noexcept == Noexcept;
}

static_assert(
    reads_as<&Spellings::plain, Spellings, int, std::tuple<char, double>, false, false>());
static_assert(reads_as<&Spellings::constant, Spellings, void, std::tuple<int>, true, false>());
static_assert(reads_as<&Spellings::quiet, Spellings, void, std::tuple<>, false, true>());
static_assert(
    reads_as<&Spellings::both, Spellings, bool, std::tuple<Object const&, float>, true, true>());

// The readers of a call operator: a handler that cannot throw names its sender, a generic one
// names none, and a body that cannot throw names its steps.
static_assert(
    std::is_same_v<impl::handler_sender_t<decltype([](Button const&) noexcept {})>, Button>);
static_assert(std::is_same_v<impl::handler_sender_t<decltype([](auto const&) {})>, void>);
static_assert(impl::iterating<decltype([](repeat<3>) noexcept { return Button{}; })>);

// Elevation, the web-card way: the shadow names what is cast and the
// translation's z lifts the caster, and only the pair reads as depth. A
// ThemeShadow is also a Shadow, so the unnamed form routes it by the base.
[[maybe_unused]] void elevation() {
    Border named{
        shadow = ThemeShadow{},
        translation = {0, 0, 32},
    };
    Border positional{ThemeShadow{}};
    Vector3 const lifted = named.translation();
    (void)lifted;
}

// The math door behind a RichEditBox: content goes in whole -- plain or RTF
// text in one SetText call, a formula as MathML -- and the mode gates what
// typing builds up. Display-only math is a read-only box with the content
// set first: a read-only document refuses programmatic writes too.
[[maybe_unused]] void rich_document(RichEditBox const& box) {
    RichEditTextDocument const document = box.document();
    document.setText(TextSetOptions::FormatRtf, LR"({\rtf1 plain, \b bold\b0 , \i italic\i0 })");
    document.setMathMode(RichEditMathMode::MathOnly);
    document.setMathML(LR"(<math xmlns="http://www.w3.org/1998/Math/MathML"><mi>x</mi></math>)");
}

// Formatting range by range -- the first wrapped WinRT *interfaces*:
// getRange hands back a TextRange (ITextRange under the I-less wxl name),
// whose characterFormat is live -- setting an effect on it changes the
// slice alone. Effects are tri-state, so a mixed range reads Undefined.
[[maybe_unused]] void range_format(RichEditTextDocument const& document) {
    TextRange const range = document.getRange(0, 3);
    TextCharacterFormat const format = range.characterFormat();
    format.bold(FormatEffect::On);
    format.italic(FormatEffect::Off);
    format.subscript(FormatEffect::Off);
    format.superscript(FormatEffect::On);
    [[maybe_unused]] FormatEffect const mixed = format.bold();
    range.characterFormat(format);
}

// A named framework brush, reached as a path like a style but assigned by
// name: a Brush fits background, borderBrush and foreground alike, so
// routing one by type alone would be a guess.
[[maybe_unused]] void named_brush() {
    Border card{
        background = brushes.Card.BackgroundFillColor.Default,
        borderBrush = brushes.Card.StrokeColorDefault,
    };
    TextBlock text{L"secondary", foreground = brushes.Text.FillColor.Secondary};
}

// A synthetic property with two homes: the same text is the hover hint
// (ToolTipService) and the automation name a screen reader speaks. It
// exists for controls whose face is a glyph rather than a word.
[[maybe_unused]] void tool_tip() {
    Button glyphOnly{toolTip = L"Switch the theme"};
    glyphOnly.toolTip(L"Switch the theme");
}

// A theme flip from code: RequestedTheme on the content root re-themes every
// control under it, ActualThemeChanged is where hand-assigned brushes are
// re-read -- an assigned brush is a value, not a reference the theme can
// retarget -- and the call form of a brush path is the themed lookup itself.
[[maybe_unused]] void theme_flip(Grid const& page) {
    page.requestedTheme(page.actualTheme() == ElementTheme::Dark ? ElementTheme::Light
                                                                 : ElementTheme::Dark);
    EventToken token = page.add_onActualThemeChanged([page](Object const&, Object const&) {
        page.background(brushes.SolidBackgroundFillColor.Base(page.actualTheme()));
    });
    page.remove_onActualThemeChanged(token);
}

// A named framework resource, reached as a path and routed by its type: a
// Style can only be the Style property.
[[maybe_unused]] void named_style() {
    TextBlock title{
        L"WinUI in C++ Without XAML!",
        styles.TextBlock.Title,
        hAlign.center,
    };
}

// A look kept at namespace scope. Values are laid out by the compiler, and
// an object inside it is a template, built where the look is applied -- a
// fresh one per application, so a child has no second parent and a shadow is
// nobody's before the runtime is up. The look with values only is a constant;
// the one carrying a template is a constant object whose template is made
// at start-up, in the pool that stands from before main().
namespace library_presets {
constexpr Preset shape{CornerRadius{12}, translation = {0, 0, 32}};

Preset const card{
    shape,
    background = brushes.Card.BackgroundFillColor.Default,
    shadow = Template<ThemeShadow>{},
};

Preset const stamp{
    Template<Button>{content = L"fresh per application"},
};
}

[[maybe_unused]] void deferred_values() {
    Border card{library_presets::card};
    Border direct{
        background = Template<SolidColorBrush>{colors.white},
        Template<ThemeShadow>{},  // unnamed: routed by what it builds
    };
    StackPanel one{library_presets::stamp};
    StackPanel two{library_presets::stamp};

    // The empty template assigned to a property is the plain object, built at
    // application like everything else a template defers; a copy of a preset
    // is a copy of its arguments.
    Border spelled{shadow = Template<ThemeShadow>{}};
    auto const copy = library_presets::card;
    Border dressed{copy};
}

// A named look, made the control it describes: the look is worn in the
// constructor with the call site's setters after it, and where a fresh
// control is wanted per use, a template builds it.
[[maybe_unused]] void built_look() {
    Border card{library_presets::card, width = 450, height = 380};
    Grid host{Template<Border>{library_presets::card, TextBlock{L"inside"}}};
    Border const copied{card};
}

// A preset: the arguments, kept in a variable and written once. Two forms,
// and each of them means one thing only -- unnamed inside braces it dresses
// the object being built, and called it dresses one that already exists. The
// object a property asks for is a template's to build.
[[maybe_unused]] void own_preset() {
    constexpr Preset glow{
        center = Point{0.33f, 0.33f},
        gradientOrigin = {0.33f, 0.33f},
        radiusX = 1.1,
        radiusY = 1.3,
    };

    Border built{background = RadialGradientBrush{
                     glow,
                     GradientStop{rgba(51, 51, 180, 0.2), offset = 0.0},
                     GradientStop{rgba(32, 32, 135, 0.125), offset = 1.0},
                 }};

    Border given{background = Template<RadialGradientBrush>{glow}};

    RadialGradientBrush plain;
    glow(plain);
}

// A template with parameters is a function returning one, and a look built
// on a preset is that preset written inside the template's braces -- unnamed,
// the same way it would be written inside the object's.
[[maybe_unused]] void templates_of_presets() {
    constexpr Preset shape{
        center = Point{0.33f, 0.33f},
        gradientOrigin = {0.33f, 0.33f},
        radiusX = 1.1,
        radiusY = 1.3,
    };

    auto const glowing = [shape](Color centre, Color edge) {
        return Template<RadialGradientBrush>{
            shape,
            GradientStop{centre, offset = 0.0},
            GradientStop{edge, offset = 1.0},
        };
    };

    Border keypad{background = glowing(rgba(51, 51, 51, 0.706), rgba(32, 32, 32, 0.53))};
    Border display{background = glowing(rgb(220, 232, 180), rgb(166, 178, 135))};
}

// A template assigned to a property is the object that property asks for, and
// always that: it is built and set, never applied to the object on the left.
// `build()` is the same step, said out loud; wearing is the preset's form.
[[maybe_unused]] void template_as_the_value() {
    auto const ink = Template<SolidColorBrush>{color = rgb(124, 135, 104)};

    Border given{borderBrush = ink};

    SolidColorBrush const brush = ink.build();
    Border same{borderBrush = brush};

    constexpr Preset inkLook{color = rgb(124, 135, 104)};
    SolidColorBrush worn{inkLook};
}

// A preset fits whatever has the members it names: the arguments are checked
// against the object wearing it, so font settings written once go on a
// Control and on a TextBlock, which share no base that has a font.
[[maybe_unused]] void preset_fits_by_its_members() {
    constexpr Preset wide{fontSize = 18.0, hAlign.stretch};

    Button key{L"7", wide};
    TextBox entry{wide};
    TextBlock label{wide};
}

// A binding written on a property, in the one form its shape allows. Bind
// takes a property the control shows and writes itself -- a binding pair
// (<wxl/BindingPairs.h>), which serves the classes derived from its own;
// BindOutput takes any property with a setter; BindInput takes a pair, both
// ways or input alone. Each written elsewhere, or written unnamed, is refused
// with the form to write instead, which no line here can show: a
// static_assert in a body is not something a requires-expression can see. The
// model is one refcounted object whose observables are its fields, not
// handles.
namespace {
struct Reading {
    std::u16string label;
    bool ready = false;
};

struct BoundModel : core::sta_refcounted {
    core::observable<std::u16string> title{u"WXL"};
    core::observable<bool> busy;
    core::observable<core::u16_text> name;
    core::observable<Color> tint;
    core::observable<int> row;
    core::observable<double> amount;
    core::observable<float> level;
    core::observable<Reading> reading;
};
}  // namespace

[[maybe_unused]] void binding_by_property() {
    auto const model = core::make_refcounted<BoundModel>();

    // Both ways: the control writes the property itself.
    TextBox{text = Bind{model->name}};
    ToggleSwitch{isOn = Bind{model->busy}};
    ComboBox{selectedIndex = Bind{model->row}};  // Selector's pair
    NumberBox{value = Bind{model->amount}};
    NumberBox{value = Bind{model->level}};  // a double property binds a float as well
    CheckBox{isChecked = Bind{model->busy}};  // ToggleButton's pair
    ToggleButton{isChecked = Bind{model->busy}};
    RadioButtons{selectedIndex = Bind{model->row}};
    Slider{value = Bind{model->amount}};
    Slider{value = Bind{model->level}};
    RatingControl{value = Bind{model->amount}};
    RatingControl{value = Bind{model->level}};
    ColorPicker{color = Bind{model->tint}};
    ToggleMenuFlyoutItem{isChecked = Bind{model->busy}};
    RadioMenuFlyoutItem{isChecked = Bind{model->busy}};
    PipsPager{selectedPageIndex = Bind{model->row}};
    PagerControl{selectedPageIndex = Bind{model->row}};

    // From the field: the control shows, pair or no pair.
    TextBlock{text = BindOutput{model->title}};   // no pair: the only form a TextBlock's text takes
    Button{isEnabled = BindOutput{model->busy}};  // named: bool alone could not choose
    ProgressRing{isActive = BindOutput{model->busy}};
    TextBox{text = BindOutput{model->name}};  // a pair, one half: what is typed stays in the box
    NumberBox{value = BindOutput{model->row}};

    // Into the field: the control writes, nothing written back.
    TextBox{text = BindInput{model->name}};
    ToggleSwitch{isOn = BindInput{model->busy}};
    ComboBox{selectedIndex = BindInput{model->row}};
    NumberBox{intermediateValue = BindInput{model->amount}};  // input alone: the number as typed
    CheckBox{isChecked = BindInput{model->busy}};
    CheckBox{isChecked = BindOutput{model->busy}};

    // Through a function: two ways when it is its own inverse, one way for any.
    CheckBox{isChecked = BindInput{model->busy, std::logical_not{}}};
    CheckBox{isChecked = Bind{model->busy, std::logical_not{}}};
    Button{isEnabled = BindOutput{model->busy, std::logical_not{}}};

    // Through a pointer to a member: the control shows one member of the value.
    TextBlock{text = BindOutput{model->reading, &Reading::label}};
    Button{isEnabled = BindOutput{model->reading, &Reading::ready}};

    // Applied to a control built already, as a ComboBox filled at run time is.
    ComboBox combo;
    Apply{combo, selectedIndex = Bind{model->row}};
}

// State the control takes only on its own terms, bound as a pair that answers
// back: isFocused on any element, verticalOffset on a ScrollViewer. Bind and
// BindInput, never BindOutput -- the field says what the control did, not what
// was asked.
[[maybe_unused]] void binding_state_pairs() {
    auto const model = core::make_refcounted<BoundModel>();

    TextBox{isFocused = Bind{model->busy}};
    Border{isFocused = Bind{model->busy}};     // a panel too: UIElement declares the focus
    Grid{isFocused = BindInput{model->busy}};  // only told
    Button{isFocused = Bind{model->busy, std::logical_not{}}};
    TextBox{schema::TextBox::isFocused = Bind{model->busy}};  // inherited from UIElement's anchor
    ScrollViewer{verticalOffset = Bind{model->amount}};
    ScrollViewer{verticalOffset = BindInput{model->amount}};
    ScrollViewer{schema::ScrollViewer::verticalOffset = Bind{model->amount}};
}

// A number is taken as the property's own type, converted at the `=`: a float
// property takes a float and an int literal, a double one widens both, and a
// binding keeps its field's type. A narrowing -- `rotation = 0.4` -- is a
// warning on the line that writes it, which no assertion here can see.
[[maybe_unused]] void numbers_take_the_property_type() {
    auto const model = core::make_refcounted<BoundModel>();

    Border{rotation = 0.4f};
    Border{rotation = 2};
    TextBlock{fontSize = 0.4f};
    TextBlock{fontSize = 2};
    Border{rotation = BindOutput{model->level}};
    TextBlock{fontSize = BindOutput{model->level}};

    static_assert(std::same_as<decltype(rotation = 2), impl::setter_op<PropertyKey::Rotation, float>>);
    static_assert(
        std::same_as<decltype(fontSize = 0.4f), impl::setter_op<PropertyKey::FontSize, double>>);
}

template <typename Tag, typename Value>
concept assignable = requires(Tag const& tag, Value value) { tag = value; };

// A character converts to a number, and is never meant as one: a tag of a number
// refuses it.
static_assert(!assignable<decltype(fontSize), char>);
static_assert(!assignable<decltype(rotation), char16_t>);

// A name the classes declare with different types takes each of them: opacity
// is a double on a UIElement and a float on a Visual. A number written as one
// of them is taken as that one; an integer is a conversion away from both, so
// it is kept as it is and the setter of the object converts it. A truth value
// is refused, a conversion away from both alike, and so is a character.
[[maybe_unused]] void numbers_on_a_name_of_several_types() {
    auto const model = core::make_refcounted<BoundModel>();
    float const level = 0.5f;
    double const amount = 0.5;

    Border{opacity = 1};
    Border{opacity = 0.5};
    Border{opacity = 0.5f};
    Border{opacity = level};
    Border{opacity = amount};
    Border{opacity = BindOutput{model->amount}};

    constexpr PropertyKey key = PropertyKey::Opacity;
    static_assert(std::same_as<decltype(opacity = 1), impl::setter_op<key, int>>);
    static_assert(std::same_as<decltype(opacity = 0.5), impl::setter_op<key, double>>);
    static_assert(std::same_as<decltype(opacity = 0.5f), impl::setter_op<key, float>>);
    static_assert(std::same_as<decltype(opacity = level), impl::setter_op<key, float>>);
    static_assert(std::same_as<decltype(opacity = amount), impl::setter_op<key, double>>);
    static_assert(std::same_as<decltype(opacity = BindOutput{model->level}),
                               impl::setter_op<key, BindOutput<float>>>);
    static_assert(!assignable<decltype(opacity), bool>);
    static_assert(!assignable<decltype(opacity), char>);
    static_assert(!assignable<decltype(opacity), char16_t>);
}

// What is not a number goes to the setter as written. Content is an Object on a
// ContentControl and a UIElement on a Flyout, and a template given to it is still
// the template in the op: the object is built where the op is applied, not at
// the `=`.
[[maybe_unused]] void objects_on_a_name_of_several_types() {
    Button{content = Template<Border>{}};
    Flyout{content = Template<Border>{}};

    static_assert(std::same_as<decltype(content = Template<Border>{}),
                               impl::setter_op<PropertyKey::Content, Template<Border>>>);
}

// A list with a brush in it takes a colour as the tag of a brush does. No name of
// the profiles is such a list, so the tag is written here.
inline constexpr impl::property_tag<PropertyKey::Background, impl::one_of<Brush, double>>
    brushOrNumber{};

[[maybe_unused]] void colours_on_a_list_with_a_brush() {
    Border{brushOrNumber = rgb(16, 18, 21)};

    static_assert(std::same_as<decltype(brushOrNumber = rgb(16, 18, 21)),
                               impl::setter_op<PropertyKey::Background, impl::solid_color_brush>>);
}

template <typename Obj, typename Value>
concept placement_takes = requires(Obj const& object, Value const& value) {
    impl::property_setter<PropertyKey::Placement>::set(object, value);
};

// Placement is a FlyoutPlacementMode on a flyout and a PlacementMode on a
// tooltip, and its values are written once: a name both have is one value the
// setter of either takes as its own, a name one of them has is that one's. The
// other refuses it.
[[maybe_unused]] void values_on_a_name_of_several_enumerations() {
    MenuFlyout{placement = placement.bottom};
    ToolTip{placement = placement.bottom};
    MenuFlyout{placement = placement.full};
    ToolTip{placement = placement.mouse};

    static_assert(static_cast<FlyoutPlacementMode>(placement.bottom) == FlyoutPlacementMode::Bottom);
    static_assert(static_cast<PlacementMode>(placement.bottom) == PlacementMode::Bottom);
    static_assert(placement.full == FlyoutPlacementMode::Full);
    static_assert(placement.mouse == PlacementMode::Mouse);
    static_assert(placement_takes<ToolTip, std::remove_cvref_t<decltype(placement.bottom)>>);
    static_assert(!placement_takes<ToolTip, std::remove_cvref_t<decltype(placement.full)>>);
}

// Copy construction must not be hijacked by the variadic constructor -- the
// single-argument case is what the constraint on it exists for.
[[maybe_unused]] void copying(Button const& source) {
    Button copy = source;
    Button moved{Button{}};
    (void)copy;
    (void)moved;
}

// The other side of that same constraint: a bare string is the whole of the
// argument list. It is a pointer once it decays, so it sits one step away
// from the exclusion the copy constructor needs, and only the pointed-to
// type tells the two apart.
[[maybe_unused]] void singleUnnamedString() {
    TextBlock caption{L"the only argument"};
    Button action{L"Click"};
    (void)caption;
    (void)action;
}

// A window's geometry is not on the window: Microsoft.UI.Xaml.Window exposes
// none of it and hands out an AppWindow, which is where the whole surface is.
[[maybe_unused]] void windowGeometry(Window const& window) {
    AppWindow const app = window.appWindow();
    app.resize({1000, 700});
    app.move({120, 80});
    app.moveAndResize({120, 80, 1000, 700});
    app.title(L"wxl");
    SizeInt32 const size = app.size();
    (void)size;
}

// Timing and the UI thread, both reached through the window's dispatcher.
[[maybe_unused]] void dispatching(Window const& window) {
    DispatcherQueue const queue = window.dispatcherQueue();
    if (!queue.hasThreadAccess()) {
        return;
    }
    DispatcherQueueTimer const timer = queue.createTimer();
    timer.interval(std::chrono::milliseconds{250});
    timer.isRepeating(true);

    // Tick carries no arguments of its own, so the handler is handed a plain
    // wxl::Object -- the same bridge out of WinRT the sender comes through.
    EventToken tick = timer.add_onTick([](Object const&, Object const&) {});
    timer.remove_onTick(tick);

    timer.start();
    timer.stop();

    // Work handed to the queue runs after this call returns, so the delegate
    // keeps a copy of the callable rather than a reference to it.
    queue.tryEnqueue([] {});
    queue.tryEnqueue(DispatcherQueuePriority::Low, [] {});

    queue.enqueueEventLoopExit();
}

// The procedural side of the same members: ordinary calls, all const.
[[maybe_unused]] void procedural(TextBlock const& text, Button const& button) {
    text.text(L"changed");
    hstring const current = text.text();
    button.content(L"Click");
    EventToken token = button.add_onClick([](Object const&, RoutedEventArgs&) {});
    button.remove_onClick(token);
    (void)current;
}

// What a handler may read out of its args, and write back into them.
//
// The view is handed over mutably on purpose -- Handled is the whole reason --
// and it is the one hierarchy in wxl whose const-ness says something. A
// wrapper of the Object family is a smart pointer, so all of its members are
// const and the qualifier means nothing; an args view *is* the object, on the
// stack for the length of the call, so reading a property is const and
// changing one is not.
[[maybe_unused]] void event_arguments(Button const& button, ListView const& list) {
    button.add_onClick([](Object const&, RoutedEventArgs& args) {
        Object const source = args.originalSource();
        (void)source;
    });

    button.add_onKeyDown([](Object const&, KeyRoutedEventArgs& args) {
        if (args.key() == VirtualKey::Enter) {
            args.handled(true);
        }
    });

    list.add_onSelectionChanged([](Object const&, SelectionChangedEventArgs& args) {
        Collection<Object> const added = args.addedItems();
        for (uint32_t i = 0; i != added.size(); ++i) {
            Object const item = added[i];
            (void)item;
        }
    });

    button.add_onKeyDown([](Object const&, KeyRoutedEventArgs const& args) {
        // A getter reaches through const, which is what lets a handler take
        // the args by const reference when it only reads them.
        if (args.key() == VirtualKey::Enter && !args.handled()) {
        }
    });
}

// And the line itself, as the member pointers say it: a property getter is
// const, a property setter is not, and neither is a method -- GetCurrentPoint
// is a call the args make on the pointer, not a property of theirs. Written as
// exact types rather than as `requires` because a member call rejected for the
// const-ness of `this` is not a substitution failure in MSVC but an error.
static_assert(
    std::is_same_v<decltype(&KeyRoutedEventArgs::key), VirtualKey (KeyRoutedEventArgs::*)() const>);

[[maybe_unused]] constexpr auto args_are_read_through_const =
    static_cast<bool (KeyRoutedEventArgs::*)() const>(&KeyRoutedEventArgs::handled);

[[maybe_unused]] constexpr auto args_are_written_through_non_const =
    static_cast<void (KeyRoutedEventArgs::*)(bool)>(&KeyRoutedEventArgs::handled);

static_assert(std::is_same_v<decltype(&PointerRoutedEventArgs::getCurrentPoint),
                             PointerPoint (PointerRoutedEventArgs::*)(UIElement const&)>);

// A wrapper that may be empty is a type of its own, so a definite one never
// has to be tested: Style means a style, core::nullable<Style> means a style
// or nothing, and only the second can be written as nullptr -- the empty
// wrapper is a null thing, and its sentinel says so (empty_is_nullptr).
[[maybe_unused]] void nullability(Style const& definite) {
    core::nullable<Style> maybe = nullptr;
    maybe = definite;
    if (maybe) {
        Style const& back = maybe.value();
        (void)back;
    }
    core::nullable<DependencyObject> widened = maybe;
    (void)widened;

    maybe = nullptr;

    // And the empty wrapper costs nothing beside the wrapper: the sentinel is
    // a null Impl, which the pointer already had room for.
    static_assert(sizeof(core::nullable<Style>) == sizeof(Style));

    // A value with no null thing in it gets no such spelling, so no type ends
    // up with two ways to say empty.
    static_assert(!std::is_constructible_v<core::nullable<core::duration>, decltype(nullptr)>);
}

// The nullable box on the way through the projection. Six element types cross
// as WinRT's IReference<T>, and five of them say "empty" in a value of their
// own rather than in a byte beside it -- so the property costs exactly what
// the bare value costs.
//
// bool is the sixth and the exception, because both of its values are meant
// and a third bit pattern read back as bool is undefined. Its selector names
// std::optional and says so, which is the whole reason every other line in
// wxl can write nullable<T> without asking: a type nobody has given a
// sentinel still does not compile.
[[maybe_unused]] void projected_nullables(ToggleButton const& toggle, ScrollViewer const& scroll,
                                          TimePicker const& time, DatePicker const& date) {
    toggle.isChecked(true);
    toggle.isChecked({});
    core::nullable<bool> const checked = toggle.isChecked();
    static_assert(std::is_same_v<std::remove_const_t<decltype(checked)>, std::optional<bool>>);

    // A coordinate that is not being changed is a NaN, which is what
    // arithmetic already answers where there is no answer.
    (void)scroll.changeView(120.0, {}, {});
    static_assert(sizeof(core::nullable<double>) == sizeof(double));
    static_assert(sizeof(core::nullable<float>) == sizeof(float));

    // A length of time empties into the maximum it never counts to; a point
    // in time into WinRT's own zero, the year 1601, which no picker offers.
    time.selectedTime(core::duration{std::chrono::hours{9}});
    core::nullable<DateTime> const picked = date.selectedDate();
    static_assert(sizeof(core::nullable<core::duration>) == sizeof(core::duration));
    static_assert(sizeof(core::nullable<DateTime>) == sizeof(DateTime));
    (void)picked;

    // A signed integer empties into its minimum and an unsigned one into its
    // maximum: the value at each end that no count arrives at meaning it.
    static_assert(sizeof(core::nullable<int32_t>) == sizeof(int32_t));
    static_assert(sizeof(core::nullable<std::size_t>) == sizeof(std::size_t));
    static_assert(!core::nullable<int32_t>{}.has_value());
    static_assert(!core::nullable<std::size_t>{}.has_value());
    static_assert(!core::nullable<double>{}.has_value());
}

// The composition surface, which the rich profile added and which is where
// an application puts drawing of its own.
//
// It is here for one reason beyond the syntax: Microsoft.UI.Composition is
// the first namespace whose metadata contains a cycle running *through
// inheritance* -- CompositionObject::startAnimation takes a
// CompositionAnimation, which derives from CompositionObject. A closure
// ordered without regard for that lands the base class in the file after
// its heirs, and this function is what stops compiling if it ever does
// again.
//
// The numerics are the second thing under test: Vector3 and Vector2 are wxl
// types, not cppwinrt's float3/float2, and a braced literal is the whole of
// writing one.
[[maybe_unused]] void composition(Compositor const& compositor, UIElement const& host) {
    ContainerVisual root = compositor.createContainerVisual();
    root.size({640, 480});

    SpriteVisual sheet = compositor.createSpriteVisual();
    sheet.size({420, 600});
    sheet.offset({40, 20, 0});
    sheet.centerPoint({0, 300, 0});
    sheet.rotationAngleInDegrees(-12.0f);
    sheet.opacity(0.0f);
    sheet.brush(compositor.createColorBrush(Color{0xFF, 0xF4, 0xEC, 0xD8}));

    // The crop is applied in the sheet's own coordinates, before its
    // rotation -- which is what makes a turned page a turned crop.
    InsetClip crop = compositor.createInsetClip();
    crop.rightInset(60.0f);
    sheet.clip(crop);

    DropShadow shadow = compositor.createDropShadow();
    shadow.blurRadius(24.0f);
    shadow.opacity(0.35f);
    shadow.offset({-8, 4, 0});
    sheet.shadow(shadow);

    // The sheen: a gradient whose opacity is animated rather than redrawn.
    CompositionLinearGradientBrush sheen = compositor.createLinearGradientBrush();
    sheen.startPoint({0, 0});
    sheen.endPoint({1, 0});

    root.children().insertAtTop(sheet);

    // Fading in with a stagger: the same pair of animations on every child,
    // the delay equal to its index times a step. The visual XAML already
    // made for the host is what an animation is started on.
    ScalarKeyFrameAnimation fade = compositor.createScalarKeyFrameAnimation();
    fade.duration(std::chrono::milliseconds{200});
    fade.delayTime(std::chrono::milliseconds{80});
    fade.insertKeyFrame(1.0f, 1.0f, compositor.createLinearEasingFunction());

    Vector3KeyFrameAnimation rise = compositor.createVector3KeyFrameAnimation();
    rise.duration(std::chrono::milliseconds{200});
    rise.insertKeyFrame(0.0f, Vector3{0, 12, 0});
    rise.insertKeyFrame(1.0f, Vector3{0, 0, 0});

    sheet.startAnimation(L"Opacity", fade);
    sheet.startAnimation(L"Offset", rise);

    Visual hosted = ElementCompositionPreview::getElementVisual(host);
    hosted.opacity(1.0f);
    ElementCompositionPreview::setElementChildVisual(host, root);
}

// Where a scroller is going, before it is shown there: the view of the next frame and the one
// a fling comes to rest at -- the parts a VirtualDrawingSurface under the content draws ahead.
void drawing_ahead_of_the_scroll() {
    ScrollViewer{onViewChanging +=
                 [](ScrollViewer const&, ScrollViewerViewChangingEventArgs& args) {
                     double const next = args.nextView().verticalOffset();
                     double const rest = args.finalView().verticalOffset();
                     bool const flung = args.isInertial();
                     (void)args.nextView().horizontalOffset();
                     (void)next;
                     (void)rest;
                     (void)flung;
                 }};
}

// Work handed to the UI thread, both ways round.
//
// tryEnqueue is the generated one: a delegate parameter is projected as a
// std::function of the delegate's own signature, so a lambda goes in
// unwrapped. It may only be called on the UI thread, because the wrapper it
// is called on may only be touched there -- which is what UiThread is for:
// taken here, copied into whatever runs elsewhere, called from there.
void handing_work_over(DispatcherQueue const& queue) {
    bool const taken = queue.tryEnqueue([] {});
    (void)taken;
    bool const soon = queue.tryEnqueue(DispatcherQueuePriority::Low, [] {});
    (void)soon;

    UiThread const ui = UiThread::current();
    auto const background = [ui](std::wstring found) {
        ui.post([found = std::move(found)] { (void)found; });
    };
    background(L"a file");
}

// A wait for a quiet moment on the UI thread's queue, which nobody names.
async::task<> after_a_quiet_moment(async::cancellation_token token) {
    WaitableTimer timer{std::chrono::milliseconds{1500}, std::move(token)};
    timer.restart();
    co_await timer;
    timer.signal();
}

// A class that is only its statics derives from Statics, not from Object.
//
// WinRT gives it System.Object as its base like every other class, and
// reading that off the metadata unchanged used to make it a wrapper: a smart
// pointer that is always null, over an Impl field that is never filled, with
// a constructor nothing can call. Nothing was wrong at runtime -- an instance
// cannot exist -- but the type said otherwise, and a core::nullable of it
// would have compiled and meant nothing.
static_assert(std::is_base_of_v<Statics, ElementCompositionPreview>,
              "a statics-only class must say so by deriving from Statics");
static_assert(!std::is_base_of_v<Object, ElementCompositionPreview>,
              "a statics-only class must not be given Object as a base");
static_assert(!std::is_default_constructible_v<ElementCompositionPreview>,
              "a statics-only class must not be constructible");

// A constructor that takes arguments. WinRT declares those on a factory
// interface rather than on the class, and reading every ActivatableAttribute
// -- not only the bare one -- is what turns them into constructors of their
// own, beside the default constructor when metadata declares one too.
[[maybe_unused]] void constructors_taking_arguments(Collection<GradientStop> const& stops) {
    LinearGradientBrush ramp{stops, 45.0};

    // An interface-typed parameter is taken as Object and asked for the
    // interface at the call, so any wrapper is accepted here and it is the
    // runtime that refuses one not implementing it. A parameter only: the
    // same property's getter would hand back an ICommand, which is not a
    // class wxl wraps, so `command` keeps its setter and has no getter at
    // all.
    Button{}.command(ramp);
}

// The two spellings of a brush resource, and the difference between them.
// The path follows the element's theme -- the setter keeps the key and
// subscribes -- while the call form reads one named theme and pins it there,
// which is what ink over a scrim that is dark in both themes needs. A literal
// brush is a literal brush and follows nothing.
[[maybe_unused]] void themed_brushes() {
    Border follows{background = brushes.Card.BackgroundFillColor.Default};
    TextBlock pinned{foreground = brushes.Text.FillColor.Primary(ElementTheme::Dark)};
    Border literal{background = SolidColorBrush{rgba(26, 26, 26, 0.424)}};

    (void)follows;
    (void)pinned;
    (void)literal;
}

// A colour written for a brush is a solid brush of that colour -- in the
// syntax only: the setter itself still takes a brush and nothing else.
constexpr Preset tinted{background = colors.white, borderBrush = Color{255, 0, 0, 0}};

[[maybe_unused]] void colours_for_brushes() {
    Border scrim{background = rgba(224, 224, 208, 0.627), tinted};
    TextBlock ink{foreground = rgb(44, 58, 28)};
    Button swatch{borderBrush = colors.gray, background = Color{255, 0, 0, 0}};
    Apply{scrim, background = colors.transparent};

    (void)ink;
    (void)swatch;
}

template <typename Value>
concept background_takes =
    requires(Border const& border, Value value) { border.background(value); };

static_assert(!std::is_convertible_v<Color, Brush>);
static_assert(!background_takes<Color>);

// Dressing an object that already exists. The runtime form of a description,
// written the way a description is: same braces, same commas, target first.
// Every route the constructor has is here, so a change that drops one of them
// from Apply -- or lets something other than a control stand first -- breaks
// this rather than an application.
[[maybe_unused]] void apply_dresses_what_already_exists(Button const& button,
                                                        StackPanel const& panel) {
    Apply{button, content = L"Switch", isEnabled = false};
    Apply{button, Margin{16, 0}, Padding{3}};
    Apply{button, styles.ButtonBase.TextBlockButton};
    Apply{button, onClick += [](Button const& self) { self.isEnabled(false); }};
    Apply{button, Preset{content = L"Switch"}};
    Apply{panel, TextBlock{L"a child goes in the same way"}};

    // A pack of one, and a pack of none: both legal, neither special.
    Apply{button, content = L"Switch"};
    Apply{button};

    static_assert(!std::is_constructible_v<Apply, decltype(content = L"a")>,
                  "the first argument is the target, and only a control may stand there");
}

// The comma is not a way to write two setters. Written in parentheses it is
// the built-in comma operator, which keeps only the last -- so wxl deletes it
// for anything that takes part in a description (see impl/member.h). What can
// silently regress is not the deletion, which is one line, but the list it
// works from: a family added later and not enrolled would quietly lose the
// guard. So these assert the enrolment, family by family.
//
// Asserting the deletion itself would be better still, and is not possible
// here: MSVC reports a deleted operator as a hard error rather than letting a
// requires-expression turn it into false.
[[maybe_unused]] void the_comma_is_not_a_separator() {
    static_assert(impl::describes<std::remove_cvref_t<decltype(content = L"a")>>);
    static_assert(impl::describes<std::remove_cvref_t<decltype(onClick += [](Button const&) {})>>);
    static_assert(impl::describes<std::remove_cvref_t<decltype(content)>>);
    static_assert(impl::describes<std::remove_cvref_t<decltype(orientation)>>);
    static_assert(impl::describes<std::remove_cvref_t<decltype(onClick)>>);
    static_assert(impl::describes<Margin>);
    static_assert(impl::describes<std::remove_cvref_t<decltype(styles.TextBlock.Header)>>);
    static_assert(impl::describes<std::remove_cvref_t<decltype(brushes.Card.StrokeColorDefault)>>);
    static_assert(impl::describes<Preset<>>);
    static_assert(impl::describes<decltype(Preset{content = L"a"})>);
    static_assert(impl::describes<Template<Button>>);

    // And what it deliberately does not reach. A control is an ordinary object,
    // and so is an enum shortcut like hAlign.center -- poisoning the comma for
    // either would reach far outside the DSL. Neither is silent anyway: the
    // comma yields something that is not callable and not a setter, so the
    // mistake surfaces at the very next step.
    static_assert(!impl::describes<Button>);
    static_assert(!impl::describes<std::remove_cvref_t<decltype(hAlign.center)>>);
}

// TextWrap shortcut: textWrap.simple, textWrap.wholeWords, textWrap.none.
[[maybe_unused]] void text_wrap_shortcut() {
    TextBlock{L"Wrapping", textWrap.simple};
    TextBlock{L"Whole words only", textWrap.wholeWords};
    TextBlock{L"No wrapping", textWrap.none};
}

template <typename T>
concept declares_orientation_tag = requires { typename T::OrientationTag; };

template <typename T>
concept declares_snaps_to_tag = requires { typename T::SnapsToTag; };

// The values of an enumeration are written once, with the enumeration, and
// every property over it carries them: the flat tag and the schema anchor
// alike, and two properties over one enumeration share the one description.
// A schema class offers its members and no helper type beside them.
[[maybe_unused]] void an_enumeration_carries_its_values_once() {
    namespace x = dsl::schema;

    static_assert(orientation.horizontal == Orientation::Horizontal);
    static_assert(x::Slider::orientation.vertical == Orientation::Vertical);
    static_assert(x::StackPanel::orientation.horizontal == orientation.horizontal);

    static_assert(horizontalContentAlignment.center == horizontalAlignment.center);
    static_assert(horizontalContentAlignment.stretch == HorizontalAlignment::Stretch);

    // A value taken from the tag is the op the enumerator itself makes.
    static_assert(std::same_as<decltype(orientation = orientation.horizontal),
                               decltype(orientation = Orientation::Horizontal)>);

    static_assert(!declares_orientation_tag<x::Slider>);
    static_assert(!declares_snaps_to_tag<x::Slider>);
}

// The schema, written the way it is meant to be read.
//
// Every element of it is compiled by the generated schema_surface.cpp beside
// this file; what is here is the part a generator cannot write down -- what
// it is *for*, and the one thing it can do that the flat vocabulary cannot.
[[maybe_unused]] void the_schema_names_the_class(LinearGradientBrush const& brush) {
    namespace x = dsl::schema;

    // Two classes declare StartPoint with different types, Point and Vector2,
    // so the flat tag takes both and `startPoint = {0, 0}` cannot compile
    // through it: the braced list fits either. Through the class the type is
    // one again.
    impl::apply_argument(brush, x::LinearGradientBrush::startPoint = {0.0f, 0.0f});
    impl::apply_argument(brush, x::LinearGradientBrush::endPoint = Point{1.0f, 1.0f});

    // And it is the same op the bare name produces, so the two forms mix
    // inside one set of braces.
    Button{
        x::Button::flyout = MenuFlyout{},
        x::ContentControl::content = L"Click me",
        content = L"and the flat name still means the same thing",
        x::Button::onClick += [](Object const&, RoutedEventArgs&) {},
    };
}

// A handler may take the sender alone. Most events carry args nobody reads, and
// the parameter written only to be ignored is what this form removes -- both
// spellings are here so neither disappears, and so the two concepts behind them
// stay disjoint (a change that let one take in the other would break this).
[[maybe_unused]] void handlers_may_drop_the_args() {
    Button{
        content = L"Switch",
        onClick += [](Button const& self) { self.isEnabled(false); },
    };

    Button{
        content = L"Switch",
        onClick += [](Button const& self, RoutedEventArgs&) { self.isEnabled(false); },
    };

    ToggleSwitch{}.add_onToggled([](ToggleSwitch const& self) { self.isOn(false); });
}

// And drop both, when the handler carries what it needs. The house callback is
// what makes that worth writing: `type` is one shared body, so each of these
// buttons captures a reference count rather than a copy of the closure.
[[maybe_unused]] void handlers_may_take_nothing_at_all() {
    core::function const type = [](wchar_t) {};

    Grid{
        [type](iterate<L"789"> key) {
            return Button{
                key.text(),
                column = key.index,
                onClick += [type, symbol = key.value] { type(symbol); },
            };
        },
    };

    Button{}.add_onClick([] {});
}

// A handler may be a member of the object that builds the tree. method() hands
// it over with the member's own parameters, so each form above is read off the
// member as it is off a lambda, and the three stay disjoint for members too.
// `this` inside a const member function points to const, and then a const
// member goes; a member that starts a coroutine keeps its task in a field.
struct HandlersAsMembers {
    void clicked(Button const& self, RoutedEventArgs& args) {
        Object const source = args.originalSource();
        (void)source;
        self.isEnabled(false);
    }
    void toggled(ToggleSwitch const& self) { self.isOn(false); }
    void pressed() noexcept {}
    void keyDown(TextBox const&, KeyRoutedEventArgs& args) const {
        if (args.key() == VirtualKey::Enter) {
            args.handled(true);
        }
    }
    void loaded(Button const& self, RoutedEventArgs&) { enabling = enables(self); }

    async::task<> enables(Button self) {
        co_await UiThread::onIdle();
        self.isEnabled(true);
    }

    void build(DispatcherQueue const& queue) {
        Button{
            onClick += method(this, &HandlersAsMembers::clicked),
            onLoaded += method(this, &HandlersAsMembers::loaded),
        };
        ToggleSwitch{onToggled += method(this, &HandlersAsMembers::toggled)};
        Button{onClick += method(this, &HandlersAsMembers::pressed)};

        // The same value after the braces, and as a delegate parameter.
        Button{}.add_onClick(method(this, &HandlersAsMembers::clicked));
        queue.tryEnqueue(method(this, &HandlersAsMembers::pressed));
    }

    void build() const { TextBox{onKeyDown += method(this, &HandlersAsMembers::keyDown)}; }

    // Last: it goes first, and stops the coroutine before anything it touches.
    async::task<> enabling;
};

// What a member may return: nothing. The refusal of anything else is a
// static_assert in method()'s body -- a hard error, never a false answer to a
// requires-expression -- so its condition is what is pinned here.
static_assert(impl::droppable_result<void>);
static_assert(!impl::droppable_result<bool> && !impl::droppable_result<async::task<>>);

// And the call reaches the member with what it was given, a reference as a
// reference, through the plain operator and the noexcept one alike -- worked
// out by the compiler on members constexpr allows, since nothing in this file
// runs.
static_assert([] {
    struct Probe {
        int sender = 0;
        constexpr void take(int const& from, int& args) {
            sender = from;
            args = 2;
        }
        constexpr void keep(int const& from, int& args) noexcept {
            sender += from;
            args += 3;
        }
    };
    Probe probe;
    int args = 0;
    method(&probe, &Probe::take)(1, args);
    method(&probe, &Probe::keep)(4, args);
    return probe.sender == 5 && args == 5;
}());

// A noexcept member makes a noexcept call, and that is what lets a member
// watch a field: an observable, like any core::event, takes only a callback
// that cannot throw. A member that may throw makes a call that may too, and
// the field refuses it. With the sender and the args, a noexcept member is
// still read as that form of handler.
struct Watcher {
    core::observable<int> count;

    void counted(int const&) noexcept {}
    void recounted(int const&) {}
    void clicked(Button const&, RoutedEventArgs&) noexcept {}

    void watch() { count.on_change(method(this, &Watcher::counted)); }
    void build() { Button{onClick += method(this, &Watcher::clicked)}; }
};

using counted_call = decltype(method(std::declval<Watcher*>(), &Watcher::counted));
using recounted_call = decltype(method(std::declval<Watcher*>(), &Watcher::recounted));
using clicked_call = decltype(method(std::declval<Watcher*>(), &Watcher::clicked));

static_assert(std::is_nothrow_invocable_v<counted_call, int const&>);
static_assert(core::invocable<counted_call, void(int const&) noexcept>);
static_assert(std::is_invocable_v<recounted_call, int const&> &&
              !std::is_nothrow_invocable_v<recounted_call, int const&>);
static_assert(!core::invocable<recounted_call, void(int const&) noexcept>);
static_assert(impl::typed_sender_handler<clicked_call, RoutedEventArgs>);

// One property, two vocabularies. SymbolIcon already knows the font, so a
// glyph is named and nothing else is said -- with WinRT's Symbol where the
// name is in it, and with wxl's FluentSymbol where it is not. QuietHours and
// Brightness are exactly that case: the moon and the sun of a theme switch,
// which Symbol never named.
[[maybe_unused]] void icons_by_name() {
    SymbolIcon fromWinrt{symbol = Symbol::Play};
    SymbolIcon fromFluent{symbol = FluentSymbol::QuietHours};

    // The positional route is Symbol's alone: a route has to be unambiguous,
    // and the property already carries the name in the assigned form.
    SymbolIcon positional{Symbol::Play};

    fromWinrt.symbol(Symbol::Pause);
    fromFluent.symbol(FluentSymbol::Brightness);

    // Asking gives back the vocabulary the metadata declares, whichever one
    // set it: there is one property under both spellings.
    static_assert(std::is_same_v<decltype(fromFluent.symbol()), Symbol>);
}

// The pointer's shape over an element is a property like any other -- set
// declaratively on a plain Border, no control subclass in between. It stands
// on UIElement.ProtectedCursor, "protected" in the metadata and reachable all
// the same: see impl/element_helpers.h.
[[maybe_unused]] void cursor_over_an_element() {
    Border sizer{width = 6, cursor = InputSystemCursorShape::SizeWestEast};

    sizer.cursor(InputSystemCursorShape::Arrow);
}

static_assert(std::is_constructible_v<LinearGradientBrush, Collection<GradientStop> const&, double>,
              "the factory interface's CreateInstanceWithGradientStopCollectionAndAngle "
              "is a constructor of its own");
static_assert(std::is_constructible_v<SolidColorBrush, Color const&>,
              "and so is the one-argument shape");
static_assert(!std::is_convertible_v<Color const&, SolidColorBrush>,
              "both are explicit, like every other wrapper constructor");

// The shape of Color itself: the ABI struct is { A, R, G, B } bytes in that
// order, and every cast across the boundary reads it that way. Asserted here
// rather than in Color.h, which half of wxl.ui includes.
static_assert(sizeof(Color) == 4);
static_assert(alignof(Color) == 1);
static_assert(offsetof(Color, A) == 0 && offsetof(Color, R) == 1 && offsetof(Color, G) == 2 &&
              offsetof(Color, B) == 3);

// The CSS functions: alpha is a fraction, rounded to the nearest byte.
static_assert(rgb(131, 50, 50) == Color{255, 131, 50, 50});
static_assert(rgba(131, 50, 50, 0.2) == Color{51, 131, 50, 50});
static_assert(rgba(0, 0, 0, 0) == colors.transparent);
static_assert(rgba(255, 255, 255, 1.0) == colors.white);

// ColorHelper::lightness() moves along OKLCH L: the ends are black and white,
// a zero step is the identity, equal steps either way from a grey land
// symmetric greys, and alpha rides along.
static_assert(ColorHelper::lightness(colors.black, 1.0) == colors.white);
static_assert(ColorHelper::lightness(colors.white, -1.0) == colors.black);
static_assert(ColorHelper::lightness(rgb(131, 50, 50), 0.0) == rgb(131, 50, 50));
static_assert(ColorHelper::lightness(colors.gray, 0.2) == rgb(189, 189, 189));
static_assert(ColorHelper::lightness(colors.gray, -0.2) == rgb(72, 72, 72));
static_assert(ColorHelper::lightness(rgba(30, 20, 200, 0.5), -0.1).A == 128);

// shade() is a product in linear light: a quarter of white is not a quarter
// of the byte, one is the identity, zero is black, a colour keeps its hue
// because the channels scale together, too much light clips at white, and
// alpha rides along.
static_assert(ColorHelper::shade(colors.white, 0.25) == rgb(137, 137, 137));
static_assert(ColorHelper::shade(colors.gray, 0.5) == rgb(92, 92, 92));
static_assert(ColorHelper::shade(rgb(200, 60, 30), 0.3) == rgb(116, 31, 13));
static_assert(ColorHelper::shade(rgb(200, 60, 30), 1.0) == rgb(200, 60, 30));
static_assert(ColorHelper::shade(rgba(200, 60, 30, 0.5), 0.0) == rgba(0, 0, 0, 0.5));
static_assert(ColorHelper::shade(colors.gray, 8.0) == colors.white);

// highlight() is a sum in linear light: the light's colour comes through and
// the surface's saturation goes; all of a white light is white.
static_assert(ColorHelper::highlight(rgb(200, 60, 30), colors.white, 0.0) == rgb(200, 60, 30));
static_assert(ColorHelper::highlight(rgb(200, 60, 30), colors.white, 0.2) == rgb(228, 136, 127));
static_assert(ColorHelper::highlight(colors.gray, rgb(255, 200, 100), 0.5) == rgb(220, 188, 144));
static_assert(ColorHelper::highlight(colors.black, colors.white, 1.0) == colors.white);

// illuminate() is Lambert: a face square to a white lamp with no ambient
// shows its albedo, a face turned away shows only the ambient, the slopes in
// between get the cosine's share of the key, and a direction's length does
// not matter.
namespace {
constexpr Light lampAbove{{0, 0, 1}, colors.white, colors.black};
constexpr Light lampAtLeft{{-1, -1, 1.2}, rgb(255, 246, 232), rgb(64, 68, 90)};
}  // namespace
static_assert(Light::illuminate(rgb(200, 60, 30), lampAbove, {0, 0, 1}) == rgb(200, 60, 30));
static_assert(Light::illuminate(rgb(200, 60, 30), lampAbove, {0, 0, 5}) == rgb(200, 60, 30));
static_assert(Light::illuminate(rgb(200, 60, 30), lampAbove, {0, 0, -1}) == colors.black);
static_assert(Light::illuminate(rgb(200, 60, 30), lampAtLeft, {0, 0, 1}) == rgb(170, 48, 22));
static_assert(Light::illuminate(rgb(200, 60, 30), lampAtLeft, {-1, -1, 1.5}) == rgb(204, 59, 28));
static_assert(Light::illuminate(rgb(200, 60, 30), lampAtLeft, {1, 1, 1.5}) == rgb(48, 9, 4));
static_assert(Light::illuminate(rgba(200, 60, 30, 0.5), lampAbove, {0, 0, 1}).A == 128);

// Relief: a raised relief turns its near face to the lamp and its far
// face away, a sunken one is the same two faces swapped, the flat top gets
// the lamp's cosine, and under a lamp straight overhead the two faces of a
// relief are one.
namespace {
constexpr auto raisedRed = Relief::raised(rgb(200, 60, 30), lampAtLeft);
constexpr auto sunkenRed = Relief::sunken(rgb(200, 60, 30), lampAtLeft);
constexpr auto underhead = Relief::raised(rgb(200, 60, 30), lampAbove);
}  // namespace
static_assert(raisedRed.nearLamp == rgb(202, 58, 28) && raisedRed.farFromLamp == rgb(81, 19, 8));
static_assert(sunkenRed.nearLamp == raisedRed.farFromLamp &&
              sunkenRed.farFromLamp == raisedRed.nearLamp);
static_assert(Relief::flat(rgb(200, 60, 30), lampAtLeft) == rgb(170, 48, 22));
static_assert(underhead.nearLamp == rgb(200, 60, 30) && underhead.farFromLamp == rgb(200, 60, 30));
static_assert(Relief::raised(rgb(200, 60, 30), lampAtLeft, 0.0).nearLamp == rgb(170, 48, 22));

// falloff() is one under the lamp, falls by the inverse square and the
// cosine, and depends only on the ratio of distance to height; under() puts
// it on the key and leaves the ambient alone.
static_assert(Relief::falloff(0.0, 1.0) == 1.0);
static_assert(Relief::falloff(1.3, 2.0) - Relief::falloff(0.65, 1.0) < 1e-12 &&
              Relief::falloff(0.65, 1.0) - Relief::falloff(1.3, 2.0) < 1e-12);
static_assert(Relief::under(rgb(200, 60, 30), lampAtLeft, 0.0, 1.0) == rgb(205, 59, 28));
static_assert(Relief::under(rgb(200, 60, 30), lampAtLeft, 0.65, 1.0) == rgb(164, 46, 21));
static_assert(Relief::under(rgb(200, 60, 30), lampAtLeft, 1.3, 1.0) == rgb(112, 29, 12));
static_assert(Relief::under(rgb(200, 60, 30), lampAtLeft, 1.3, 2.0) == rgb(164, 46, 21));

// towards() places a lamp by two angles: azimuth the classical way round the
// screen, elevation above it. Straight overhead is the viewer's axis.
namespace {
constexpr auto leftUp = Direction::from_angles(120, 60);
constexpr auto overhead = Direction::from_angles(45, 90);
constexpr bool close_to(double value, double expected) {
    return value - expected < 1e-6 && expected - value < 1e-6;
}
}  // namespace
static_assert(close_to(leftUp.x, -0.25) && close_to(leftUp.y, -0.433013) &&
              close_to(leftUp.z, 0.866025));
static_assert(close_to(overhead.x, 0.0) && close_to(overhead.y, 0.0) && close_to(overhead.z, 1.0));

// across() is the gradient axis of a face under the lamp: from the lamp's
// side to the far side through the middle, whatever the lamp's height; a
// lamp overhead leaves both ends in the middle.
namespace {
constexpr auto axisLeftUp =
    Relief::across({Direction::from_angles(120, 60), colors.white, colors.black});
constexpr auto axisLow =
    Relief::across({Direction::from_angles(120, 20), colors.white, colors.black});
constexpr auto axisOverhead =
    Relief::across({Direction::from_angles(120, 90), colors.white, colors.black});
}  // namespace
static_assert(close_to(axisLeftUp.startX, 0.25) && close_to(axisLeftUp.startY, 0.5 - 0.4330127) &&
              close_to(axisLeftUp.endX, 0.75) && close_to(axisLeftUp.endY, 0.5 + 0.4330127));
static_assert(close_to(axisLow.startX, axisLeftUp.startX) &&
              close_to(axisLow.endY, axisLeftUp.endY));
static_assert(close_to(axisOverhead.startX, 0.5) && close_to(axisOverhead.endY, 0.5));

// cushion() samples the shoulder ring by ring, outermost first: at full rise
// the outer ring leans nearly to the side and the far side of it is all but
// in the ambient, the inner ring is nearly the top; at no rise every ring is
// the top; a negative rise swaps the sides.
namespace {
constexpr Light lampHigh{Direction::from_angles(120, 60), rgb(255, 246, 232), rgb(64, 68, 90)};
constexpr auto cushionRed = Relief::cushion<4>(rgb(200, 60, 30), lampHigh, 1.0);
constexpr auto flatRed = Relief::cushion<4>(rgb(200, 60, 30), lampHigh, 0.0);
constexpr auto dentRed = Relief::cushion<4>(rgb(200, 60, 30), lampHigh, -1.0);
constexpr Color topRed = Relief::flat(rgb(200, 60, 30), lampHigh);
}  // namespace
static_assert(topRed == rgb(192, 55, 26));
static_assert(cushionRed[0].nearLamp == rgb(192, 55, 26) &&
              cushionRed[0].farFromLamp == rgb(48, 9, 4));
static_assert(cushionRed[1].nearLamp == rgb(204, 59, 28) &&
              cushionRed[1].farFromLamp == rgb(134, 36, 16));
static_assert(cushionRed[2].nearLamp == rgb(204, 59, 28) &&
              cushionRed[2].farFromLamp == rgb(167, 47, 22));
static_assert(cushionRed[3].nearLamp == rgb(198, 57, 27) &&
              cushionRed[3].farFromLamp == rgb(186, 53, 25));
static_assert(flatRed[0].nearLamp == topRed && flatRed[0].farFromLamp == topRed &&
              flatRed[3].nearLamp == topRed && flatRed[3].farFromLamp == topRed);
static_assert(dentRed[0].nearLamp == cushionRed[0].farFromLamp &&
              dentRed[0].farFromLamp == cushionRed[0].nearLamp &&
              dentRed[2].nearLamp == cushionRed[2].farFromLamp);

// Color is a bare aggregate, the way it crosses the ABI.
static_assert(std::is_aggregate_v<Color> && std::is_trivially_copyable_v<Color> &&
              sizeof(Color) == 4);

// What a string parameter takes: whatever carries a zero after its text, in
// either unit, and an hstring a getter hands back -- so a value read off one
// control goes straight into another without naming a unit at the call site.
static_assert(std::is_convertible_v<char16_t const*, hstring_param>);
static_assert(std::is_convertible_v<char16_t const (&)[5], hstring_param>);
static_assert(std::is_convertible_v<wchar_t const*, hstring_param>);
static_assert(std::is_convertible_v<wchar_t const (&)[5], hstring_param>);
static_assert(std::is_convertible_v<std::u16string const&, hstring_param>);
static_assert(std::is_convertible_v<std::wstring const&, hstring_param>);
// Checked text that owns its buffer, straight: a model's u16_text reaches a
// control's setter without a step back through units.
static_assert(std::is_convertible_v<core::u16_text const&, hstring_param>);
static_assert(std::is_convertible_v<hstring const&, hstring_param>);
static_assert(std::is_convertible_v<zstring_view, hstring_param>);

// And no plain view: it promises nothing about the unit after its end, and the
// type exists to carry that promise. A view goes through assume_terminated().
static_assert(!std::is_convertible_v<std::u16string_view, hstring_param>);
static_assert(!std::is_convertible_v<std::wstring_view, hstring_param>);
static_assert(!std::is_convertible_v<core::u16_view, hstring_param>);

// Nor anything implicit going back: a parameter that decayed into a wchar_t
// view would be a silent way back to the unit this type exists to leave.
static_assert(!std::is_convertible_v<hstring_param, std::wstring_view>);

// The value types that keep a string take the same spellings in one step.
static_assert(std::is_convertible_v<char16_t const (&)[5], Uri>);
static_assert(std::is_convertible_v<std::u16string const&, Uri>);
static_assert(std::is_convertible_v<hstring const&, Uri>);
static_assert(!std::is_convertible_v<std::u16string_view, Uri>);
static_assert(std::is_convertible_v<char16_t const (&)[5], FontFamily>);
static_assert(std::is_convertible_v<char16_t const (&)[5], ImageSource>);
static_assert(std::is_convertible_v<std::u16string const&, ImageSource>);

// The rename between the two units is only sound while they are the same
// width. MSVC gives that; the standard promises it nowhere, which is the whole
// reason the code is moving off wchar_t.
static_assert(sizeof(wchar_t) == sizeof(char16_t));
// Awaiting an event. A wait is named by what makes it rather than spelled
// out: the type carries how it reaches its event, and that is the factory's
// business, not the caller's.
using click_wait_t = decltype(on_event<EventKey::Click>(std::declval<Button const&>()));
using split_click_wait_t = decltype(on_event<EventKey::Click>(std::declval<SplitButton const&>()));
using layout_wait_t = decltype(on_event<EventKey::LayoutUpdated>(std::declval<TextBlock const&>()));

// The args are read off the signature of the element's own add_onX, and the
// pair below is why that is not the same as reading them off the event: one
// EventKey, two classes, two different args. A subscription made from a key
// has to land on the right one of them.
static_assert(std::is_same_v<click_wait_t::args_t, RoutedEventArgs>);
static_assert(std::is_same_v<split_click_wait_t::args_t, SplitButtonClickEventArgs>);

// And they reach the coroutine exactly as they reach a handler: event args
// proper by non-const reference, since they are a non-copyable view the
// runtime owns; the events whose args are an ordinary object by const
// reference, since that one is a wrapper the coroutine must not replace.
static_assert(std::is_same_v<click_wait_t::args_ref_t, RoutedEventArgs&>);
static_assert(std::is_same_v<layout_wait_t::args_ref_t, Object const&>);

// Neither copied nor moved: the handler it hands the element captures it by
// address, so a proxy that could travel would leave that handler pointing at
// where it used to be. on() still returns one by value -- a prvalue
// initialises the variable in place.
static_assert(!std::is_copy_constructible_v<click_wait_t>);
static_assert(!std::is_move_constructible_v<click_wait_t>);
static_assert(std::is_same_v<decltype(on_event<EventKey::Click>(std::declval<Button const&>())),
                             click_wait_t>);

// The two forms differ in one thing and say so in their result: the bare wait
// hands back the args, the answering one hands back either the args or the
// reason there are none. A reference cannot be the value of an expected, so
// it travels wrapped; the other side is the stop reason.
using key_wait_t = decltype(on_event<EventKey::KeyDown>(std::declval<Button const&>()));

static_assert(
    std::is_same_v<decltype(std::declval<key_wait_t&>().operator co_await().await_resume()),
                   KeyRoutedEventArgs&>);
static_assert(std::is_same_v<decltype(std::declval<key_wait_t&>().next().await_resume()),
                             std::expected<std::reference_wrapper<KeyRoutedEventArgs>,
                                           async::stop_reason>>);

// And the reason costs no more than the error it may carry: stopped by request
// is a null exception_ptr.
static_assert(sizeof(async::stop_reason) == sizeof(std::exception_ptr));

// A string is not a stop reason until it is asked to be one.
static_assert(!std::is_convertible_v<char const*, async::stop_reason>);
static_assert(!std::is_convertible_v<std::string_view, async::stop_reason>);
static_assert(std::is_constructible_v<async::stop_reason, char const*>);

// Awaiting writes down who is waiting, so the proxy has to be non-const.
// Only the positive half is assertable: the const overload is deleted rather
// than absent, and MSVC reports naming a deleted function as an error where
// it stands instead of letting a requires-expression answer false.
static_assert(requires(click_wait_t& proxy) { proxy.operator co_await(); });

// Under a cancellation token, given last as wxl::async's operations take one.
// The proxy is a type of its own, so a wait made without a token keeps every
// byte it had: its awaiter is the pointer and two flags it can be cancelled
// through, and the idle wait the size it was.
using cancelable_click_wait_t = decltype(on_event<EventKey::Click>(
    std::declval<Button const&>(), std::declval<async::cancellation_token>()));
using cancelable_key_wait_t = decltype(on_event<EventKey::KeyDown>(
    std::declval<Button const&>(), std::declval<async::cancellation_token>()));

static_assert(
    std::is_same_v<cancelable_click_wait_t,
                   impl::cancelable_event_awaitable<impl::keyed_event<EventKey::Click, Button>>>);
static_assert(std::is_same_v<decltype(onClick(std::declval<Button const&>(),
                                              std::declval<async::cancellation_token>())),
                             cancelable_click_wait_t>);
static_assert(!std::is_move_constructible_v<cancelable_click_wait_t>);

// The two forms answer as they do without a token.
static_assert(std::is_same_v<
              decltype(std::declval<cancelable_key_wait_t&>().operator co_await().await_resume()),
              KeyRoutedEventArgs&>);
static_assert(std::is_same_v<decltype(std::declval<cancelable_key_wait_t&>().next().await_resume()),
                             key_wait_t::next_result_t>);

static_assert(sizeof(click_wait_t::awaiter) == 2 * sizeof(void*));
static_assert(sizeof(cancelable_click_wait_t) ==
              sizeof(click_wait_t) + sizeof(async::cancellation_token));
static_assert(async::detail::cancelable_awaiter<click_wait_t::awaiter>);
static_assert(async::detail::cancelable_awaiter<UiThread::Idle>);
static_assert(sizeof(UiThread::Idle) == sizeof(impl::event_wait) +
                                            sizeof(core::intrusive_ptr<impl::idle_link>) +
                                            sizeof(void*));

// The protocol itself, proven the only way it can be: by a coroutine. Never
// called -- activating a control needs a runtime that is not up here -- and
// compiled, which is the whole of what this file does.
struct probe_task {
    struct promise_type {
        probe_task get_return_object() const noexcept { return {}; }
        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_always final_suspend() const noexcept { return {}; }
        void return_void() const noexcept {}
        void unhandled_exception() const noexcept {}
    };
};

[[maybe_unused]] probe_task awaiting_an_event(Button button, TextBlock text) {
    // Kept in a variable: one subscription, waited on as many times as the
    // loop goes round.
    auto clicks = on_event<EventKey::Click>(button);

    while (true) {
        RoutedEventArgs& args = co_await clicks;
        static_cast<void>(args);

        // Answering the framework from inside a coroutine, which is the one
        // thing this whole shape has to keep possible: the handler resumes
        // the frame before it returns, so Handled set here is set in time.
        KeyRoutedEventArgs& key = co_await on_event<EventKey::KeyDown>(button);
        key.handled(true);

        // And as a temporary: one wait, subscribed and unsubscribed by the
        // full expression it is written in. The temporary outlives the
        // suspension inside it, which is what makes this legal.
        Object const& sender = co_await on_event<EventKey::LayoutUpdated>(text);
        static_cast<void>(sender);

        // The same wait named by its own two members instead of by a key --
        // for an event the generated vocabulary does not name, and for the
        // events of a type, which have no object to be asked of.
        RoutedEventArgs& again =
            co_await on_event(button, &Button::add_onClick, &Button::remove_onClick);
        static_cast<void>(again);

        // The same wait through the tag the braces use -- `onClick += handler`
        // to handle, `onClick(button)` to await -- and through the schema,
        // which also checks that a Button is what the tag was declared on.
        static_assert(
            std::is_same_v<decltype(onClick(button)), decltype(on_event<EventKey::Click>(button))>);
        RoutedEventArgs& tagged = co_await onClick(button);
        static_cast<void>(tagged);
        RoutedEventArgs& owned = co_await schema::Button::onClick(button);
        static_cast<void>(owned);

        // The answering form, for a body that must let nothing escape. Stopped
        // by request is the plain end; stopped by an error carries what went
        // wrong, so nothing is lost by not throwing.
        if (auto const got = co_await on_event<EventKey::KeyUp>(button).next()) {
            got->get().handled(true);
        } else if (got.error().stopped_due_to_error()) {
            std::rethrow_exception(got.error().error());
        }
    }
}

// The same waits under a cancellation token, given last -- and the idle wait
// beside them, with and without one.
[[maybe_unused]] probe_task awaiting_under_a_token(Button button, async::cancellation_token token) {
    auto clicks = on_event<EventKey::Click>(button, token);
    RoutedEventArgs& args = co_await clicks;
    static_cast<void>(args);

    RoutedEventArgs& tagged = co_await onClick(button, token);
    static_cast<void>(tagged);
    RoutedEventArgs& owned = co_await schema::Button::onClick(button, token);
    static_cast<void>(owned);
    RoutedEventArgs& again =
        co_await on_event(button, &Button::add_onClick, &Button::remove_onClick, token);
    static_cast<void>(again);

    if (auto const got = co_await on_event<EventKey::KeyUp>(button, token).next()) {
        got->get().handled(true);
    }

    co_await UiThread::onIdle(token);
    co_await UiThread::onIdle();
}

// An asynchronous method of a wrapper has its form with a token beside it, the token taken last:
// cancelled, the operation is asked to stop and the co_await ends with the cancellation -- for a
// dialog, the dialog closes. The form without one is as it was.
static_assert(std::is_same_v<decltype(std::declval<ContentDialog const&>().showAsync()),
                             async::task<ContentDialogResult>>);
static_assert(std::is_same_v<decltype(std::declval<ContentDialog const&>().showAsync(
                                 std::declval<async::cancellation_token>())),
                             async::task<ContentDialogResult>>);

[[maybe_unused]] async::task<bool> asking_under_a_token(ContentDialog dialog,
                                                        async::cancellation_token token) {
    co_return co_await dialog.showAsync(std::move(token)) == ContentDialogResult::Primary;
}

// An event of the application's own, a core::event, awaited under a token: one wait per
// co_await, either spelling of the signature, and the arguments -- one as it is, several as a
// tuple, none as nothing -- as values, but for a reference to non-const, the channel the
// coroutine answers through.
using count_wait_t = decltype(on_event(std::declval<core::event<void(int)>&>(),
                                       std::declval<async::cancellation_token>()));

static_assert(std::is_same_v<decltype(std::declval<count_wait_t&>().await_resume()), int>);
static_assert(std::is_same_v<decltype(on_event(std::declval<core::event<void(int&) noexcept>&>(),
                                               std::declval<async::cancellation_token>())
                                          .await_resume()),
                             int&>);
static_assert(
    std::is_same_v<decltype(on_event(std::declval<core::event<void(std::u16string const&, int)>&>(),
                                     std::declval<async::cancellation_token>())
                                .await_resume()),
                   std::tuple<std::u16string, int>>);
static_assert(
    std::is_same_v<decltype(on_event(std::declval<core::event<void(std::u16string const&)>&>(),
                                     std::declval<async::cancellation_token>())
                                .await_resume()),
                   std::u16string>);
static_assert(std::is_same_v<decltype(on_event(std::declval<core::event<void()>&>(),
                                               std::declval<async::cancellation_token>())
                                          .await_resume()),
                             void>);
static_assert(!std::is_move_constructible_v<count_wait_t>);
static_assert(
    async::detail::cancelable_awaiter<impl::core_event_wait<core::event<void(int)>, int>>);

// A reference to const comes as a copy, so a type whose copy would be sliced to its static
// type -- or that has none -- is refused where the wait is made, with a static_assert that says
// what to declare instead. The assert stands on a concept, which is what can be checked here:
// ordinary types pass, a polymorphic one does not, and a reference to non-const is never copied.
struct sliced_answer {
    virtual ~sliced_answer() = default;
};

static_assert(impl::invocation_answerable<int>);
static_assert(impl::invocation_answerable<std::u16string const&>);
static_assert(impl::invocation_answerable<sliced_answer&>);
static_assert(!impl::invocation_answerable<sliced_answer const&>);

[[maybe_unused]] probe_task awaiting_an_event_of_the_model(
    core::event<void(int)>& counted,
    core::event<void(std::u16string const&, int) noexcept>& renamed, core::event<void()>& reset,
    async::cancellation_token token) {
    int const count = co_await on_event(counted, token);
    static_cast<void>(count);

    auto const [name, times] = co_await on_event(renamed, token);
    static_cast<void>(name);
    static_cast<void>(times);

    co_await on_event(reset, token);
}

// UiThread::onIdle -- written by hand: awaited in a task, between steps of work.
[[maybe_unused]] ::wxl::async::task<> idle_between_steps(int steps) {
    while (steps-- > 0) {
        co_await ::wxl::UiThread::onIdle();
    }
}

}  // namespace

// A list bound to an observable_list: itemsSource = BindOutput{list, fn} on the four list controls, named and through
// the schema; a read-only list; an item held by pointer whose own field is bound inside its element; the item
// back from what a click hands out (boundItem); the selection as a field of any Selector.
namespace {
struct BoardCard : ::wxl::core::sta_refcounted {
    explicit BoardCard(::wxl::core::u16_text t) : title(std::move(t)) {}

    ::wxl::core::u16_text const title;
    ::wxl::core::observable<::wxl::core::u16_text> progress;
};

struct Board {
    ::wxl::core::observable_list<::wxl::core::intrusive_ptr<BoardCard>> cards;
    ::wxl::core::observable_list<::wxl::core::u16_text> words;
    ::wxl::core::observable<int> chosen;

    ::wxl::core::observable_list<::wxl::core::u16_text const>& shown() { return words; }
};
}  // namespace

[[maybe_unused]] void bound_lists(Board& board) {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    auto const row = [](core::u16_text const& word) { return TextBlock{text = word}; };

    ListView{itemsSource = BindOutput{board.words, row}};
    GridView{itemsSource = BindOutput{board.words, row}};
    ItemsRepeater{itemsSource = BindOutput{board.words, row}};
    ItemsView{itemsSource = BindOutput{board.words, [](core::u16_text const& word) {
                                           return ItemContainer{child = TextBlock{text = word}};
                                       }}};
    ListView{itemsSource = BindOutput{board.shown(), row}};  // read-only: only the model changes it
    ListView{schema::ItemsControl::itemsSource = BindOutput{board.words, row}};

    ListView{
        isItemClickEnabled = true,
        itemsSource = BindOutput{board.cards,
                                 [](core::intrusive_ptr<BoardCard> const& card) {
                                     return StackPanel{
                                         TextBlock{text = card->title},
                                         TextBlock{text = BindOutput{card->progress}}};
                                 }},
        onItemClick +=
        [&cards = board.cards](ListView const&, ItemClickEventArgs& args) {
            if (core::intrusive_ptr<BoardCard> const* card = boundItem(cards, args.clickedItem())) {
                (*card)->progress.set(core::u16_text{u"opened"});
            }
        },
        selectedIndex = Bind{board.chosen},
    };
    ListView{itemsSource = BindOutput{board.words, row}, selectedIndex = BindInput{board.chosen}};
    GridView{itemsSource = BindOutput{board.words, row}, selectedIndex = BindOutput{board.chosen}};
    FlipView{selectedIndex = Bind{board.chosen}};
    ComboBox{selectedIndex = Bind{board.chosen}};
}
