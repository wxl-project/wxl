#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "wxl.gen.h"

// Last of the headers: it imports std and wxl.core, and MSVC takes no standard
// header after that.
#include "test_directory.h"

import std;
import wxl.core;

namespace {

// u8, so that the path reads the directory as UTF-8 and not in the ANSI code page.
std::filesystem::path const profiles_dir{u8"" WXL_GEN_PROFILES_DIR};
// The components the profiles name, as the build finds them.
std::filesystem::path const components_dir{u8"" WXL_UI_COMPONENTS_DIR};

// The list the configure of this build wrote: the packages its restore resolved
// and the Windows SDK of its projection.
std::filesystem::path const restored_packages_file{u8"" WXL_RESTORED_PACKAGES};

// The generator's output for one test, in a directory of the test's own,
// emptied before and removed after.
class stale_files : public ::testing::Test {
protected:
    void SetUp() override {
        dir_ = std::filesystem::path{wxl::core::test_directory().native()};
        std::filesystem::remove_all(dir_);
        std::filesystem::create_directories(dir_);
    }

    void TearDown() override { std::filesystem::remove_all(dir_); }

    output out() const { return {dir_, 2}; }

    std::filesystem::path dir_;
};

}  // namespace

// A run takes away what an earlier run wrote and this one did not, by the names a
// listing of the directory hands out. A stale wrapper named in Cyrillic --
// "устаревший.h", spelled out so that the source's own encoding does not matter --
// goes, and so does a part of an earlier run with more parts; a file of the same
// kind that does not carry the generator's banner stays, and so does what the run
// wrote itself: its two parts and the schema's test beside them.
TEST_F(stale_files, one_named_in_cyrillic_is_removed) {
    auto const stale = dir_ / L"\u0443\u0441\u0442\u0430\u0440\u0435\u0432\u0448\u0438\u0439.h";
    auto const own = dir_ / L"\u0441\u0432\u043E\u0439.h";  // "свой.h"
    auto const stale_part = dir_ / L"parts" / L"wrapper-02.cpp";
    std::ofstream{stale} << gen::banner << "#pragma once\n";
    std::ofstream{own} << "#pragma once\n";
    std::filesystem::create_directories(stale_part.parent_path());
    std::ofstream{stale_part} << gen::banner;

    auto const profiles = resolve_profiles({profiles_dir / "base.winxl-profile"},
                                           load_restored_packages(restored_packages_file),
                                           {.directories = {components_dir}});
    auto const types = load_type_map(profiles_dir / "types.json");
    run(profiles, types, {}, out(), nullptr);

    EXPECT_FALSE(std::filesystem::exists(stale));
    EXPECT_FALSE(std::filesystem::exists(stale_part));
    EXPECT_TRUE(std::filesystem::exists(own));
    EXPECT_TRUE(std::filesystem::exists(dir_ / L"parts" / L"wrapper-00.cpp"));
    EXPECT_TRUE(std::filesystem::exists(dir_ / L"parts" / L"wrapper-01.cpp"));
    EXPECT_TRUE(std::filesystem::exists(dir_ / L"parts" / L"schema_surface.cpp"));
}

namespace {

std::string read_file(std::filesystem::path const& path) {
    std::ifstream file{path, std::ios::binary};
    return {std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

}  // namespace

// The binding pairs of a run are those of the classes it generates: the base profile
// has ToggleSwitch and ToggleButton and no NumberBox, so BindingPairs.h has the first
// two and nothing of the third. Each pair is the owner's binding_pair, handed by
// property_binder to every class derived from the owner, and a pair of the metadata
// has its bodies in a part, over impl::bind_pair; a bound member's pair calls its
// function in the header.
TEST_F(stale_files, binding_pairs_are_those_of_the_classes_the_run_generates) {
    auto const profiles = resolve_profiles({profiles_dir / "base.winxl-profile"},
                                           load_restored_packages(restored_packages_file),
                                           {.directories = {components_dir}});
    auto const types = load_type_map(profiles_dir / "types.json");
    run(profiles, types, {}, out(), nullptr);

    auto const header = read_file(dir_ / L"BindingPairs.h");
    for (auto&& part : {"struct binding_pair<PropertyKey::IsOn, ToggleSwitch>",
                        "struct binding_pair<PropertyKey::IsChecked, ToggleButton>",
                        "requires std::derived_from<Control, ToggleButton>",
                        "struct property_binder<PropertyKey::IsChecked, Control> : "
                        "binding_pair<PropertyKey::IsChecked, ToggleButton>",
                        "struct binding_pair<PropertyKey::IsFocused, UIElement>",
                        "impl::apply_bind_focus(control, field, asked);"}) {
        EXPECT_NE(header.find(part), std::string::npos) << part;
    }
    EXPECT_EQ(header.find("NumberBox"), std::string::npos);
    EXPECT_EQ(header.find("winrt"), std::string::npos);

    std::string parts;
    for (auto&& name : {L"wrapper-00.cpp", L"wrapper-01.cpp"}) {
        parts += read_file(dir_ / L"parts" / name);
    }
    EXPECT_NE(parts.find("impl::bind_pair<EventKey::Checked, EventKey::Unchecked>("),
              std::string::npos);
    EXPECT_NE(parts.find("c.isChecked().value_or(false)"), std::string::npos);

    auto const surface = read_file(dir_ / L"parts" / L"schema_surface.cpp");
    EXPECT_NE(surface.find("ToggleSwitch_isOn_paired_bool"), std::string::npos);
}

// A double property binds as a nullable double as well: Slider's Value, a pair of the base
// profile, declares a bind() for that field beside double and float, reads the control as it is
// -- a NaN converts to empty -- and writes an empty field to it as NaN. The surface test binds
// the field through the anchor.
TEST_F(stale_files, a_double_property_binds_a_nullable_double) {
    auto const profiles = resolve_profiles({profiles_dir / "base.winxl-profile"},
                                           load_restored_packages(restored_packages_file),
                                           {.directories = {components_dir}});
    auto const types = load_type_map(profiles_dir / "types.json");
    run(profiles, types, {}, out(), nullptr);

    auto const header = read_file(dir_ / L"BindingPairs.h");
    EXPECT_NE(header.find("static void bind(Slider const& control, "
                          "core::observable<core::nullable<double>>& field,"),
              std::string::npos);

    std::string parts;
    for (auto&& name : {L"wrapper-00.cpp", L"wrapper-01.cpp"}) {
        parts += read_file(dir_ / L"parts" / name);
    }
    EXPECT_NE(parts.find("-> core::nullable<double> { return c.value(); }"), std::string::npos);
    EXPECT_NE(parts.find("c.value(value.value_or(std::numeric_limits<double>::quiet_NaN()))"),
              std::string::npos);

    auto const surface = read_file(dir_ / L"parts" / L"schema_surface.cpp");
    EXPECT_NE(surface.find("Slider_value_paired_core__nullable_double_"), std::string::npos);
}

namespace {

std::string utf8(std::filesystem::path const& path) {
    return std::string{wxl::core::unicode::to_utf8(path).chars()};
}

// What the call stops with; a failure of the test when it does not stop.
template <typename F>
std::string stops_with(F&& call) {
    try {
        call();
    } catch (std::runtime_error const& error) {
        return error.what();
    }
    ADD_FAILURE() << "the call had to stop with an error";
    return {};
}

// Whether `text` holds every one of `parts`, each named when it is missing.
::testing::AssertionResult says(std::string_view text, std::vector<std::string> const& parts) {
    for (auto&& part : parts) {
        if (text.find(part) == std::string_view::npos) {
            return ::testing::AssertionFailure() << "no \"" << part << "\" in: " << text;
        }
    }
    return ::testing::AssertionSuccess();
}

// A component <directory>/<name>/component.json with these keys after its name, and the header
// Card.h its descriptions name. Returns the manifest as the run names it.
std::filesystem::path write_component(std::filesystem::path const& directory,
                                      std::string_view name, std::string_view keys) {
    auto const component = directory / name;
    std::filesystem::create_directories(component);
    std::ofstream{component / L"Card.h"} << "#pragma once\n";
    std::ofstream{component / L"component.json", std::ios::binary}
        << "{\"name\": \"" << name << "\", " << keys << "}";
    return std::filesystem::canonical(component / L"component.json");
}

// A profile in `directory` extending the base profile, with these keys besides.
std::filesystem::path write_profile(std::filesystem::path const& directory, std::string_view keys) {
    auto base = utf8(profiles_dir / "base.winxl-profile");
    std::ranges::replace(base, '\\', '/');
    auto const file = directory / L"p.winxl-profile";
    std::ofstream{file, std::ios::binary} << "{\"extends\": [\"" << base << "\"], " << keys << "}";
    return file;
}

}  // namespace

// A description the run cannot hold to the metadata or to the other descriptions stops the run
// before anything is written, with the manifest and the member named: a class the metadata
// generates as well, a class two components describe, a type of a component that is not a
// dependency, a word that names no type, a type the vocabulary cannot take, a property named as
// an attached one and of another type, and a type of the metadata it does not declare.
TEST_F(stale_files, a_description_the_run_cannot_hold_stops_it) {
    std::string_view const card = R"("classes": {"Card": {"header": "Card.h"}})";
    struct refusal {
        std::string card;  // the keys of component card
        std::string deck;  // of component deck, selected as well when not empty
        std::string says;
        bool by_deck = false;  // the manifest named is deck's, not card's
    };
    std::vector<refusal> const refusals{
        {R"("classes": {"Button": {"header": "Card.h"}})", {},
         "Button is described here and generated from the metadata as well, both under "
         "namespace wxl"},
        {std::string{card}, std::string{card},
         "Card is described here and by component 'card'", true},
        {std::string{card},
         R"("classes": {"Deck": {"header": "Card.h", "properties": {"Top": "Card"}}})",
         "Deck.Top: the type 'Card' is described by component 'card', which is not among the "
         "dependencies of component 'deck'",
         true},
        {R"("classes": {"Card": {"header": "Card.h", "base": "Kard"}})", {},
         "Card.(base): 'Kard' is no type of the metadata read, nor one a selected component "
         "describes"},
        {R"("classes": {"Card": {"header": "Card.h", "properties": {"Args": "Microsoft.UI.Xaml.RoutedEventArgs"}}})",
         {}, "Card.Args: Microsoft.UI.Xaml.RoutedEventArgs is an event-args view"},
        {R"("classes": {"Card": {"header": "Card.h", "properties": {"Row": "Double"}}})", {},
         "Card.Row: Row is the name of an attached property of Grid, whose tag takes int32_t"},
        {R"("classes": {"Card": {"header": "Card.h", "properties": {"Icon": "Microsoft.UI.Xaml.Controls.NoSuchIcon"}}})",
         {},
         "the descriptions of the components name types the metadata read does not declare:\n"
         "  Card.Icon: Microsoft.UI.Xaml.Controls.NoSuchIcon (component 'card', "},
    };
    auto const restored = load_restored_packages(restored_packages_file);
    auto const types = load_type_map(profiles_dir / "types.json");
    int at = 0;
    for (auto&& [card_keys, deck_keys, message, by_deck] : refusals) {
        auto const directory = dir_ / std::to_wstring(at++);
        auto const components = directory / L"components";
        auto const card_manifest = write_component(components, "card", card_keys);
        auto const deck_manifest = deck_keys.empty()
                                       ? std::filesystem::path{}
                                       : write_component(components, "deck", deck_keys);
        auto const profile = write_profile(directory, deck_keys.empty()
                                                          ? R"("components": ["card"])"
                                                          : R"("components": ["card", "deck"])");

        auto const profiles =
            resolve_profiles({profile}, restored, {.directories = {components_dir, components}});
        EXPECT_TRUE(says(stops_with([&] { run(profiles, types, {}, out(), nullptr); }),
                         {utf8(by_deck ? deck_manifest : card_manifest), message}))
            << message;
        EXPECT_FALSE(std::filesystem::exists(dir_ / L"schema.h")) << message;
    }
}

// A binding pair of the type map whose wrapper cannot make it stops the run with the type map
// and the pair named: a property the wrapper has no setter of, and one of a type no rule binds.
TEST_F(stale_files, a_binding_pair_the_wrapper_cannot_make_stops_the_run) {
    auto const profiles = resolve_profiles({profiles_dir / "base.winxl-profile"},
                                           load_restored_packages(restored_packages_file),
                                           {.directories = {components_dir}});
    struct refusal {
        type_map::binding_pair pair;
        std::string says;
    };
    std::vector<refusal> const refusals{
        {{"Microsoft.UI.Xaml.FrameworkElement", "ActualWidth", {"Loaded"}},
         "binding pair Microsoft.UI.Xaml.FrameworkElement.ActualWidth: the wrapper of "
         "FrameworkElement has no setter of ActualWidth"},
        {{"Microsoft.UI.Xaml.FrameworkElement", "Margin", {"Loaded"}},
         "binding pair Microsoft.UI.Xaml.FrameworkElement.Margin: no rule binds a field to a "
         "property of type Thickness"},
    };
    for (auto&& [pair, message] : refusals) {
        auto types = load_type_map(profiles_dir / "types.json");
        types.binding_pairs.push_back(pair);
        EXPECT_TRUE(says(stops_with([&] { run(profiles, types, {}, out(), nullptr); }),
                         {utf8(types.source), message}));
    }
}

// Every wrapper lives in the one namespace wxl, so the compositor of the Windows platform listed
// beside the one of WinUI, which the base profile reaches through the window, would be a second
// definition of the first: the run stops and names both.
TEST_F(stale_files, a_class_named_as_another_stops_the_run) {
    auto const profile =
        write_profile(dir_, R"("types": {"Windows.UI.Composition.Compositor": {"members": []}})");
    auto const profiles = resolve_profiles(
        {profile}, load_restored_packages(restored_packages_file), {.directories = {components_dir}});
    EXPECT_TRUE(says(stops_with([&] {
                         run(profiles, load_type_map(profiles_dir / "types.json"), {}, out(),
                             nullptr);
                     }),
                     {"two classes map to the same flat wxl name 'Compositor'",
                      "Microsoft.UI.Composition.Compositor", "Windows.UI.Composition.Compositor"}));
}
