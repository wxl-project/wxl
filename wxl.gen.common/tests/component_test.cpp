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

// What a component requires joins the roots of the walk, and the root says which component
// brought it; a type a profile lists too stays the profile's.
TEST(component, its_required_types_are_roots_of_the_walk) {
    auto const directory = scratch(L"roots");
    auto const components = directory / L"components";
    write_component(components, "window",
                    R"("requires": {"types": {"Microsoft.UI.Xaml.Window": {"members": ["Title"]},
                                              "Microsoft.UI.Xaml.Controls.Button": {}}})");
    auto const profile = write_profile(directory, R"("window")",
                                       R"("types": {"Microsoft.UI.Xaml.Controls.Button": {}})");

    auto const set = resolve_profiles({profile}, no_packages(directory), {.directories = {components}});
    ASSERT_TRUE(set.types.contains("Microsoft.UI.Xaml.Window"));
    EXPECT_EQ(set.types.at("Microsoft.UI.Xaml.Window"), member_filter::allow({"Title"}));
    EXPECT_EQ(set.listed_by.at("Microsoft.UI.Xaml.Window").component, "window");
    EXPECT_EQ(set.listed_by.at("Microsoft.UI.Xaml.Controls.Button").component, "");
    // A component asks for no brushes; the profile's default of all of them stays.
    EXPECT_EQ(set.brushes, member_filter::all());

    std::filesystem::remove_all(directory);
}

// The components selected are those the chain names, closed over `needs`, each after the
// ones it needs; `named_components` keeps the chain's own list.
TEST(component, needs_are_followed_transitively) {
    auto const directory = scratch(L"needs");
    auto const components = directory / L"components";
    write_component(components, "core");
    write_component(components, "elements", R"("needs": ["core"])");
    write_component(components, "window", R"("needs": ["elements"])");
    auto const base = directory / L"base.winxl-profile";
    write(base, R"({"components": ["elements"]})");
    auto const profile = write_profile(directory, R"("window")", R"("extends": ["base"])");

    auto const set = resolve_profiles({profile}, no_packages(directory), {.directories = {components}});
    EXPECT_EQ(names_of(set.components), (std::vector<std::string>{"core", "elements", "window"}));
    EXPECT_EQ(set.named_components, (std::vector<std::string>{"elements", "window"}));

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

TEST(component, a_cycle_of_needs_is_refused) {
    auto const directory = scratch(L"cycle");
    auto const components = directory / L"components";
    write_component(components, "a", R"("needs": ["b"])");
    write_component(components, "b", R"("needs": ["a"])");
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
                     {"found twice", "one", "two"}));

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

    auto const required = write_component(directory, "elements", R"("requires": {"brushes": []})");
    EXPECT_TRUE(says(stops_with([&] { load_component(required); }), {utf8(required), "'brushes'"}));

    std::filesystem::remove_all(directory);
}

// The build's own list of the chain's components is held to the chain's, as sets and before
// `needs` adds to them.
TEST(component, the_list_of_the_build_must_match_the_profiles) {
    auto const directory = scratch(L"cross_check");
    auto const components = directory / L"components";
    write_component(components, "core");
    write_component(components, "window", R"("needs": ["core"])");
    auto const profile = write_profile(directory, R"("window")");
    auto const restored = no_packages(directory);

    EXPECT_NO_THROW(resolve_profiles({profile}, restored,
                                     {.directories = {components},
                                      .expected = std::vector<std::string>{"window"}}));
    EXPECT_TRUE(says(stops_with([&] {
                         resolve_profiles({profile}, restored,
                                          {.directories = {components},
                                           .expected = std::vector<std::string>{"core", "window"}});
                     }),
                     {"build:    core, window", "profiles: window"}));

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
    component_search const search{.directories = {common}, .common_directory = common};

    EXPECT_NO_THROW(resolve_profiles(
        {write_including_profile(directory, R"("window")", "../window/window_helpers.h")},
        restored, search));
    auto const profile =
        write_including_profile(directory, R"("window")", "../grid/grid_helpers.h");
    EXPECT_TRUE(says(stops_with([&] { resolve_profiles({profile}, restored, search); }),
                     {utf8(profile), "../grid/grid_helpers.h"}));

    std::filesystem::remove_all(directory);
}

// What a component's own `requires` includes is checked as well, and reported against its
// manifest.
TEST(component, an_include_of_a_component_names_the_manifest) {
    auto const directory = scratch(L"includes_component");
    auto const components = directory / L"components";
    auto const common = directory / L"common";
    auto const manifest = write_component(
        components, "window",
        R"("requires": {"types": {"Microsoft.UI.Xaml.Window": {"syntheticMembers": {"MinSize": )"
        R"({"type": "Windows.Graphics.SizeInt32", "function": "f", "include": "../missing.h"}}}}})");
    std::filesystem::create_directories(common);
    auto const profile = write_profile(directory, R"("window")");

    EXPECT_TRUE(says(stops_with([&] {
                         resolve_profiles({profile}, no_packages(directory),
                                          {.directories = {components}, .common_directory = common});
                     }),
                     {utf8(std::filesystem::canonical(manifest)),
                      "Microsoft.UI.Xaml.Window.MinSize", "../missing.h"}));

    std::filesystem::remove_all(directory);
}
