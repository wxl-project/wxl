#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "profile.h"

import wxl.core;

namespace {

std::string utf8(std::filesystem::path const& path) {
    return std::string{wxl::core::unicode::to_utf8(path).chars()};
}

// A directory of the test's own under %TEMP%, empty.
std::filesystem::path scratch(std::wstring_view name) {
    auto const directory =
        std::filesystem::temp_directory_path() / L"wxl.gen.common.component" / name;
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    return directory;
}

void write(std::filesystem::path const& file, std::string_view text) {
    std::filesystem::create_directories(file.parent_path());
    std::ofstream{file, std::ios::binary} << text;
}

// A restore with no packages and a Windows SDK nothing here opens.
restored_packages no_packages(std::filesystem::path const& directory) {
    auto const winmd = directory / L"sdk" / L"Windows.Foundation.FoundationContract.winmd";
    write(winmd, "");
    auto winmd_text = utf8(winmd);
    std::ranges::replace(winmd_text, '\\', '/');
    auto const file = directory / L"restored-packages.json";
    write(file, R"({"packages": [], "windowsSdk": {"version": "10.0.26100.0", "metadata": [")" +
                    winmd_text + R"("]}})");
    return load_restored_packages(file);
}

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

::testing::AssertionResult says(std::string_view text, std::vector<std::string> const& parts) {
    for (auto&& part : parts) {
        if (text.find(part) == std::string_view::npos) {
            return ::testing::AssertionFailure() << "no \"" << part << "\" in: " << text;
        }
    }
    return ::testing::AssertionSuccess();
}

// A component <directory>/<name>/component.json with these keys after its name.
std::filesystem::path write_component(std::filesystem::path const& directory,
                                      std::string_view name, std::string_view keys = {}) {
    auto const manifest = directory / name / L"component.json";
    write(manifest, std::string{"{\"name\": \""} + std::string{name} + "\"" +
                        (keys.empty() ? "" : ", ") + std::string{keys} + "}");
    return manifest;
}

// A profile selecting these components, with whatever else `keys` adds.
std::filesystem::path write_profile(std::filesystem::path const& directory,
                                    std::string_view components, std::string_view keys = {}) {
    auto const file = directory / L"p.winxl-profile";
    write(file, std::string{"{\"components\": ["} + std::string{components} + "]" +
                    (keys.empty() ? "" : ", ") + std::string{keys} + "}");
    return file;
}

std::vector<std::string> names_of(std::vector<component> const& components) {
    std::vector<std::string> names;
    for (auto&& comp : components) {
        names.push_back(comp.name);
    }
    return names;
}

}  // namespace

// What a component imports joins the roots of the walk, and the root says which component
// brought it; a type a profile lists too stays the profile's.
TEST(component, its_imported_types_are_roots_of_the_walk) {
    auto const directory = scratch(L"roots");
    auto const components = directory / L"components";
    write_component(components, "window",
                    R"("imports": {"types": {"Microsoft.UI.Xaml.Window": {"members": ["Title"]},
                                              "Microsoft.UI.Xaml.Controls.Button": {}}})");
    auto const profile = write_profile(
        directory, R"("window")",
        R"("types": {"Microsoft.UI.Xaml.Controls.Button": {"styles": ["AccentButtonStyle"]}},
           "brushes": ["X"])");

    auto const set = resolve_profiles({profile}, no_packages(directory),
                                      {.directories = {components}});
    ASSERT_TRUE(set.types.contains("Microsoft.UI.Xaml.Window"));
    EXPECT_EQ(set.types.at("Microsoft.UI.Xaml.Window"), member_filter::allow({"Title"}));
    EXPECT_EQ(set.listed_by.at("Microsoft.UI.Xaml.Window").component, "window");
    EXPECT_EQ(set.listed_by.at("Microsoft.UI.Xaml.Controls.Button").component, "");
    // A component asks for no brushes and no styles: what the profile narrowed stays narrow, and
    // a type only the component lists keeps all of its styles by having no entry.
    EXPECT_EQ(set.brushes, member_filter::allow({"X"}));
    EXPECT_EQ(set.styles.at("Microsoft.UI.Xaml.Controls.Button"),
              member_filter::allow({"AccentButtonStyle"}));
    EXPECT_FALSE(set.styles.contains("Microsoft.UI.Xaml.Window"));

    std::filesystem::remove_all(directory);
}

// The components selected are those the chain names, closed over `dependencies`, each after
// its dependencies; `named_components` keeps the chain's own list.
TEST(component, dependencies_are_followed_transitively) {
    auto const directory = scratch(L"dependencies");
    auto const components = directory / L"components";
    write_component(components, "core");
    write_component(components, "elements", R"("dependencies": ["core"])");
    write_component(components, "window", R"("dependencies": ["elements"])");
    auto const base = directory / L"base.winxl-profile";
    write(base, R"({"components": ["elements"]})");
    auto const profile = write_profile(directory, R"("window")", R"("extends": ["base"])");

    auto const set = resolve_profiles(
        {profile}, no_packages(directory),
        {.directories = {components}});
    EXPECT_EQ(names_of(set.components), (std::vector<std::string>{"core", "elements", "window"}));
    ASSERT_EQ(set.named_components.size(), 2u);
    EXPECT_EQ(set.named_components[0].name, "elements");
    EXPECT_EQ(set.named_components[0].by, base);
    EXPECT_EQ(set.named_components[1].name, "window");
    EXPECT_EQ(set.named_components[1].by, profile);

    std::filesystem::remove_all(directory);
}

TEST(component, a_missing_component_names_the_profile_and_the_directories) {
    auto const directory = scratch(L"missing");
    auto const components = directory / L"components";
    std::filesystem::create_directories(components);
    auto const profile = write_profile(directory, R"("window")");

    EXPECT_TRUE(says(stops_with([&] {
                         resolve_profiles({profile}, no_packages(directory),
                                          {.directories = {components}});
                     }),
                     {utf8(profile), "'window'", utf8(components)}));

    std::filesystem::remove_all(directory);
}

TEST(component, a_cycle_of_dependencies_is_refused) {
    auto const directory = scratch(L"cycle");
    auto const components = directory / L"components";
    write_component(components, "a", R"("dependencies": ["b"])");
    write_component(components, "b", R"("dependencies": ["a"])");
    auto const profile = write_profile(directory, R"("a")");

    EXPECT_TRUE(says(stops_with([&] {
                         resolve_profiles({profile}, no_packages(directory),
                                          {.directories = {components}});
                     }),
                     {"cycle", "a -> b -> a"}));

    std::filesystem::remove_all(directory);
}

TEST(component, a_name_found_in_two_directories_is_refused) {
    auto const directory = scratch(L"twice");
    auto const first = write_component(directory / L"one", "window");
    auto const second = write_component(directory / L"two", "window");
    auto const profile = write_profile(directory, R"("window")");

    EXPECT_TRUE(says(stops_with([&] {
                         resolve_profiles({profile}, no_packages(directory),
                                          {.directories = {directory / L"one", directory / L"two"}});
                     }),
                     {"found twice", utf8(std::filesystem::canonical(first)),
                      utf8(std::filesystem::canonical(second))}));

    std::filesystem::remove_all(directory);
}

// The file system finds window/ by the name "Window" too; the request is refused rather than
// selecting the component twice.
TEST(component, a_name_requested_in_another_case_is_refused) {
    auto const directory = scratch(L"case");
    auto const components = directory / L"components";
    write_component(components, "window");
    auto const profile = write_profile(directory, R"("Window")");

    EXPECT_TRUE(says(stops_with([&] {
                         resolve_profiles({profile}, no_packages(directory),
                                          {.directories = {components}});
                     }),
                     {utf8(profile), "'Window'", "'window'"}));

    std::filesystem::remove_all(directory);
}

TEST(component, a_name_other_than_its_directory_is_refused) {
    auto const directory = scratch(L"misnamed");
    auto const manifest = directory / L"window" / L"component.json";
    write(manifest, R"({"name": "windows"})");

    EXPECT_TRUE(says(stops_with([&] { load_component(manifest); }),
                     {utf8(manifest), "\"windows\"", "\"window\""}));

    std::filesystem::remove_all(directory);
}

TEST(component, a_key_a_manifest_does_not_have_is_refused) {
    auto const directory = scratch(L"unknown_key");
    auto const manifest = write_component(directory, "window", R"("sauces": [])");
    EXPECT_TRUE(says(stops_with([&] { load_component(manifest); }), {utf8(manifest), "'sauces'"}));

    auto const imports = write_component(directory, "elements", R"("imports": {"brushes": []})");
    EXPECT_TRUE(says(stops_with([&] { load_component(imports); }), {utf8(imports), "'brushes'"}));

    std::filesystem::remove_all(directory);
}

// The build's own list of the chain's components is held to the chain's, as sets and before
// `dependencies` adds to them.
TEST(component, the_list_of_the_build_must_match_the_profiles) {
    auto const directory = scratch(L"cross_check");
    auto const components = directory / L"components";
    write_component(components, "core");
    write_component(components, "grid", R"("dependencies": ["core"])");
    write_component(components, "window", R"("dependencies": ["core"])");
    auto const base = directory / L"base.winxl-profile";
    write(base, R"({"components": ["window"]})");
    auto const profile = write_profile(directory, R"("window", "grid")", R"("extends": ["base"])");
    auto const restored = no_packages(directory);

    auto const set = resolve_profiles({profile}, restored, {.directories = {components}});

    // Order and repetition take no part.
    EXPECT_NO_THROW(check_build_components(set, {"window", "grid", "window"}));
    EXPECT_TRUE(says(stops_with([&] { check_build_components(set, {"core", "grid", "window"}); }),
                     {"build:    core, grid, window", "profiles: grid, window",
                      "window: " + utf8(base), "grid: " + utf8(profile)}));

    std::filesystem::remove_all(directory);
}

// A name listed twice in "components", "setterMethods", "dependencies", "sources" or "libraries"
// is a mistake, not a repetition to fold.
TEST(component, a_name_listed_twice_is_refused) {
    auto const directory = scratch(L"listed_twice");
    auto const profile = write_profile(directory, R"("window", "window")");
    EXPECT_TRUE(says(stops_with([&] { load_profile(profile); }),
                     {utf8(profile), "'components'", "\"window\" twice"}));

    auto const setters = write_profile(
        directory, R"("window")",
        R"("types": {"Microsoft.UI.Xaml.Window": {"setterMethods": ["SetTitleBar", "SetTitleBar"]}})");
    EXPECT_TRUE(says(stops_with([&] { load_profile(setters); }),
                     {utf8(setters), "'setterMethods'", "\"SetTitleBar\" twice"}));

    auto const manifest = write_component(directory, "grid", R"("dependencies": ["core", "core"])");
    EXPECT_TRUE(says(stops_with([&] { load_component(manifest); }),
                     {utf8(manifest), "'dependencies'", "\"core\" twice"}));

    auto const sources = write_component(directory, "items", R"("sources": ["a.cpp", "a.cpp"])");
    EXPECT_TRUE(says(stops_with([&] { load_component(sources); }),
                     {utf8(sources), "'sources'", "\"a.cpp\" twice"}));

    auto const libraries =
        write_component(directory, "html", R"("libraries": ["wxl::html", "wxl::html"])");
    EXPECT_TRUE(says(stops_with([&] { load_component(libraries); }),
                     {utf8(libraries), "'libraries'", "\"wxl::html\" twice"}));

    std::filesystem::remove_all(directory);
}

// An empty list of the build is compared like any other: it matches only a chain naming none.
TEST(component, no_list_of_the_build_is_an_empty_one) {
    auto const directory = scratch(L"cross_check_empty");
    auto const components = directory / L"components";
    write_component(components, "window");
    auto const profile = write_profile(directory, R"("window")");
    auto const restored = no_packages(directory);

    auto const set = resolve_profiles({profile}, restored, {.directories = {components}});
    EXPECT_TRUE(says(stops_with([&] { check_build_components(set, {}); }),
                     {"build:    (none)", "profiles: window"}));

    auto const none = directory / L"none.winxl-profile";
    write(none, "{}");
    EXPECT_NO_THROW(check_build_components(resolve_profiles({none}, restored), {}));

    std::filesystem::remove_all(directory);
}

// A component describes the classes and the enumerations it writes by hand: members by kind,
// each with its type word, a property bound alone in the object form.
TEST(component, its_descriptions_are_read_in_the_order_of_the_file) {
    auto const directory = scratch(L"descriptions");
    write(directory / L"controls" / L"Card.h", "");
    write(directory / L"controls" / L"Alignment.h", "");
    auto const manifest = write_component(directory, "controls", R"(
        "classes": {
            "Card": {
                "header": "Card.h",
                "base": "Microsoft.UI.Xaml.Controls.ContentControl",
                "properties": {"Header": "Object", "Width": {"type": "Double", "set": "bind"}},
                "collections": {"Items": "Microsoft.UI.Xaml.UIElement"},
                "events": {"Click": "Microsoft.UI.Xaml.RoutedEventArgs"}
            },
            "CardArgs": {"header": "Card.h"}
        },
        "enums": {"Alignment": {"header": "Alignment.h", "values": ["Right", "Left"]}})");

    auto const comp = load_component(manifest);
    ASSERT_EQ(comp.classes.size(), 2u);
    auto const& card = comp.classes.front();
    EXPECT_EQ(card.name, "Card");
    EXPECT_EQ(card.header, "Card.h");
    EXPECT_EQ(card.base, "Microsoft.UI.Xaml.Controls.ContentControl");
    ASSERT_EQ(card.properties.size(), 2u);
    EXPECT_EQ(card.properties[0].name, "Header");
    EXPECT_EQ(card.properties[0].type, "Object");
    EXPECT_FALSE(card.properties[0].bound);
    EXPECT_EQ(card.properties[1].name, "Width");
    EXPECT_EQ(card.properties[1].type, "Double");
    EXPECT_TRUE(card.properties[1].bound);
    ASSERT_EQ(card.collections.size(), 1u);
    EXPECT_EQ(card.collections[0].type, "Microsoft.UI.Xaml.UIElement");
    ASSERT_EQ(card.events.size(), 1u);
    EXPECT_EQ(card.events[0].name, "Click");
    EXPECT_TRUE(comp.classes[1].base.empty());
    EXPECT_TRUE(comp.classes[1].properties.empty());
    ASSERT_EQ(comp.enums.size(), 1u);
    EXPECT_EQ(comp.enums[0].values, (std::vector<std::string>{"Right", "Left"}));

    std::filesystem::remove_all(directory);
}

TEST(component, a_description_it_cannot_read_stops_the_run) {
    auto const directory = scratch(L"descriptions_refused");
    write(directory / L"controls" / L"Card.h", "");
    struct refusal {
        std::string keys;
        std::string says;
    };
    std::vector<refusal> const refusals{
        {R"("classes": {"Card": {"header": "Card.h", "events2": {}}})",
         "class Card: 'events2' is not a key of this entry"},
        {R"("classes": {"Card": {"header": "Missing.h"}})",
         "class Card: 'header' \"Missing.h\" is no file of the component"},
        {R"("classes": {"Card": {"header": "../Card.h"}})", "is no file of the component"},
        {R"("classes": {"Card": {"base": "Object"}})", "every class Card needs a string 'header'"},
        {R"("classes": {"Card": {"header": "Card.h", "properties": {"Header": ""}}})",
         "Card.Header: the type must be a type word"},
        {R"("classes": {"Card": {"header": "Card.h", "properties": {"W": {"type": "Double", "set": "always"}}}})",
         "Card.W: 'set' says how the property is set"},
        {R"("classes": {"Card": {"header": "Card.h", "events": {"Click": {"type": "Object", "set": "bind"}}}})",
         "Card.Click: the type must be a type word"},
        {R"("enums": {"Alignment": {"header": "Card.h", "values": []}})",
         "enumeration Alignment: 'values' names no value"},
        {R"("enums": {"Alignment": {"header": "Card.h", "values": ["A", "A"]}})",
         "'values' names \"A\" twice"},
    };
    for (auto&& [keys, message] : refusals) {
        auto const manifest = write_component(directory, "controls", keys);
        EXPECT_TRUE(says(stops_with([&] { load_component(manifest); }), {utf8(manifest), message}))
            << keys;
    }

    std::filesystem::remove_all(directory);
}

namespace {

// A profile one of whose types has a synthetic member declared in `include`.
std::filesystem::path write_including_profile(std::filesystem::path const& directory,
                                              std::string_view components,
                                              std::string_view include) {
    return write_profile(
        directory, components,
        std::string{R"("types": {"Microsoft.UI.Xaml.Window": {"syntheticMembers": {"MinSize": )"
                    R"({"type": "Windows.Graphics.SizeInt32", "function": "f", "include": ")"} +
            std::string{include} + "\"}}}}");
}

}  // namespace

// "../X.h" is X relative to the common directory or to a selected component's directory; a
// plain name is a header the generator writes.
TEST(component, an_include_is_found_where_it_may_be) {
    auto const directory = scratch(L"includes_found");
    auto const components = directory / L"components";
    auto const common = directory / L"common";
    write_component(components, "window");
    write(components / L"window" / L"window_helpers.h", "");
    write(common / L"impl" / L"core.h", "");
    auto const restored = no_packages(directory);
    component_search const search{.directories = {components},
                                  .common_directory = common,
                                  .generated_includes = {"FluentSymbol.h"}};

    for (auto&& include : {"../window_helpers.h", "../impl/core.h", "FluentSymbol.h"}) {
        EXPECT_NO_THROW(resolve_profiles(
            {write_including_profile(directory, R"("window")", include)}, restored, search))
            << include;
    }

    // Without a common directory nothing is checked.
    EXPECT_NO_THROW(resolve_profiles(
        {write_including_profile(directory, R"("window")", "../missing.h")}, restored,
        {.directories = {components}}));

    std::filesystem::remove_all(directory);
}

TEST(component, an_include_found_nowhere_names_the_member_and_the_profile) {
    auto const directory = scratch(L"includes_missing");
    auto const components = directory / L"components";
    auto const common = directory / L"common";
    write_component(components, "window");
    write_component(components, "grid");
    write(components / L"grid" / L"grid_helpers.h", "");
    write(common / L"core.h", "");
    write(directory / L"outside.h", "");
    auto const restored = no_packages(directory);
    component_search const search{.directories = {components},
                                  .common_directory = common,
                                  .generated_includes = {"FluentSymbol.h"}};

    // A sibling of a selected component, a file climbing out of the directories, a missing
    // file, and a plain name the generator does not write, though a file of that name exists.
    for (auto&& include :
         {"../grid/grid_helpers.h", "../../outside.h", "../missing.h", "core.h"}) {
        auto const profile = write_including_profile(directory, R"("window")", include);
        EXPECT_TRUE(says(stops_with([&] { resolve_profiles({profile}, restored, search); }),
                         {utf8(profile), "Microsoft.UI.Xaml.Window.MinSize", include}));
    }

    std::filesystem::remove_all(directory);
}

// The library's include directory holding the components themselves: a file of a component the
// profiles do not select is refused although it lies in the common directory.
TEST(component, an_include_of_an_unselected_component_is_refused_in_the_common_directory) {
    auto const directory = scratch(L"includes_nested");
    auto const common = directory / L"src";
    write_component(common, "window");
    write_component(common, "grid");
    write(common / L"window" / L"window_helpers.h", "");
    write(common / L"grid" / L"grid_helpers.h", "");
    auto const restored = no_packages(directory);
    component_search const search{
        .directories = {common}, .common_directory = common};

    EXPECT_NO_THROW(resolve_profiles(
        {write_including_profile(directory, R"("window")", "../window/window_helpers.h")},
        restored, search));
    auto const profile =
        write_including_profile(directory, R"("window")", "../grid/grid_helpers.h");
    EXPECT_TRUE(says(stops_with([&] { resolve_profiles({profile}, restored, search); }),
                     {utf8(profile), "../grid/grid_helpers.h"}));

    std::filesystem::remove_all(directory);
}

// A component may repeat what a profile defines for a type, and may not define it otherwise.
TEST(component, a_definition_other_than_the_profiles_is_refused) {
    auto const directory = scratch(L"conflict");
    auto const components = directory / L"components";
    auto const restored = no_packages(directory);
    struct definition {
        std::string_view what;
        std::string_view profiles;
        std::string_view other;
    };
    for (auto&& [what, profiles, other] : std::initializer_list<definition>{
             {"synthetic member Microsoft.UI.Xaml.Window.MinSize",
              R"("syntheticMembers": {"MinSize": {"type": "Windows.Graphics.SizeInt32", "function": "f", "include": "../a.h"}})",
              R"("syntheticMembers": {"MinSize": {"type": "Windows.Graphics.SizeInt32", "function": "g", "include": "../a.h"}})"},
             {"fromText of Microsoft.UI.Xaml.Window",
              R"("fromText": {"function": "f", "include": "../a.h"})",
              R"("fromText": {"function": "g", "include": "../a.h"})"},
             {"replaced call Microsoft.UI.Xaml.Window.Close",
              R"("replacedCalls": {"Close": {"function": "f", "include": "../a.h"}})",
              R"("replacedCalls": {"Close": {"function": "f", "include": "../b.h"}})"},
         }) {
        auto const type = [](std::string_view entry) {
            return std::string{R"("types": {"Microsoft.UI.Xaml.Window": {)"} + std::string{entry} +
                   "}}";
        };
        auto const profile = write_profile(directory, R"("window")", type(profiles));
        component_search const search{.directories = {components}};

        write_component(components, "window", "\"imports\": {" + type(profiles) + "}");
        EXPECT_NO_THROW(resolve_profiles({profile}, restored, search)) << what;

        auto const manifest =
            write_component(components, "window", "\"imports\": {" + type(other) + "}");
        EXPECT_TRUE(says(stops_with([&] { resolve_profiles({profile}, restored, search); }),
                         {utf8(std::filesystem::canonical(manifest)), utf8(profile),
                          std::string{what}}));
    }

    std::filesystem::remove_all(directory);
}

// What a component's own `imports` includes is checked as well, and reported against its
// manifest.
TEST(component, an_include_of_a_component_names_the_manifest) {
    auto const directory = scratch(L"includes_component");
    auto const components = directory / L"components";
    auto const common = directory / L"common";
    auto const manifest = write_component(
        components, "window",
        R"("imports": {"types": {"Microsoft.UI.Xaml.Window": {"syntheticMembers": {"MinSize": )"
        R"({"type": "Windows.Graphics.SizeInt32", "function": "f", "include": "../missing.h"}}}}})");
    std::filesystem::create_directories(common);
    auto const profile = write_profile(directory, R"("window")");

    EXPECT_TRUE(says(stops_with([&] {
                         resolve_profiles({profile}, no_packages(directory),
                                          {.directories = {components},
                                           .common_directory = common});
                     }),
                     {utf8(std::filesystem::canonical(manifest)),
                      "Microsoft.UI.Xaml.Window.MinSize", "../missing.h"}));

    std::filesystem::remove_all(directory);
}
