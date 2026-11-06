#include <gtest/gtest.h>

#include <deque>
#include <filesystem>
#include <format>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "crawl.h"

import wxl.core;

namespace {

// u8, so that the path reads the directory as UTF-8 and not in the ANSI code page.
std::filesystem::path const profiles_dir{u8"" WXL_GEN_PROFILES_DIR};

// The list the configure of this build wrote: the packages its restore resolved
// and the Windows SDK of its projection.
std::filesystem::path const restored_packages_file{u8"" WXL_RESTORED_PACKAGES};

std::string utf8(std::filesystem::path const& path) {
    return std::string{wxl::core::unicode::to_utf8(path).chars()};
}

// What the walk stops with; a failure of the test when it does not stop.
std::string walk_error(profile_set const& profiles, type_map const& types) {
    std::vector<std::string> files;
    for (auto&& file : profiles.metadata) {
        files.push_back(utf8(file));
    }
    winmd::reader::cache const db{files};
    try {
        crawl(profiles, types, db);
    } catch (std::runtime_error const& error) {
        return error.what();
    }
    ADD_FAILURE() << "a type the metadata does not declare has to stop the walk";
    return {};
}

// A profile that reads Microsoft.UI.Xaml.winmd alone and lists `types`, written
// into a directory of the test's own. A walk that runs to the end needs the
// Windows metadata as well, which the signatures and attributes of WinUI name.
std::filesystem::path write_profile(std::wstring_view test, std::string_view types,
                                    bool windows_metadata = false) {
    auto const directory = std::filesystem::temp_directory_path() / L"wxl.gen.common.crawl" / test;
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    auto const profile = directory / L"p.winxl-profile";
    std::ofstream{profile} << R"({"packages": [{"id": "Microsoft.WindowsAppSDK.WinUI",
                                                "metadata": ["Microsoft.UI.Xaml.winmd"]}],
                                 "windowsMetadata": )"
                           << (windows_metadata ? "true" : "false") << R"(,
                                 "types": )"
                           << types << "}";
    return profile;
}

template <typename Types>
std::vector<std::string> names_of(Types const& types) {
    std::vector<std::string> names;
    for (auto&& type : types) {
        names.push_back(std::format("{}.{}", type.TypeNamespace(), type.TypeName()));
    }
    return names;
}

}  // namespace

// winmd orders types by the addresses of their file's tables first, and the
// heap lays a cache's files out in a different order every time. Caches of the
// same files, all alive at once, stand for separate runs: their walks agree
// only if nothing in the walk orders types that way.
TEST(crawl, order_depends_on_the_metadata_alone) {
    auto const types = load_type_map(profiles_dir / "types.json");
    auto const profiles = resolve_profiles({profiles_dir / "rich.winxl-profile"},
                                           load_restored_packages(restored_packages_file));

    std::vector<std::string> files;
    for (auto&& file : profiles.metadata) {
        files.push_back(std::string{wxl::core::unicode::to_utf8(file).chars()});
    }
    std::deque<winmd::reader::cache> caches;
    auto const walk = [&] { return crawl(profiles, types, caches.emplace_back(files)); };

    auto const first = walk();
    for (int run = 1; run < 8; ++run) {
        auto const next = walk();
        ASSERT_EQ(names_of(first.ordered), names_of(next.ordered)) << "walk " << run;
        ASSERT_EQ(names_of(first.boundary), names_of(next.boundary)) << "walk " << run;
    }
}

// A type a profile lists and the metadata does not declare stops the walk -- as
// a profile that projects WrapPanel stops on a release whose WinUI has none: the
// error names the type, the profile that lists it and the Windows App SDK
// release of the restore.
TEST(crawl, a_type_the_metadata_does_not_declare_stops_the_walk) {
    auto const profile = write_profile(L"undeclared_type", R"({
        "Microsoft.UI.Xaml.Controls.Button": {},
        "Microsoft.UI.Xaml.Controls.NoSuchPanel": {},
        "Windows.UI.NoSuchColor": {}
    })");
    auto const restored = load_restored_packages(restored_packages_file);
    auto const message = walk_error(resolve_profiles({profile}, restored),
                                    load_type_map(profiles_dir / "types.json"));

    restored_package const* const release = restored.find("Microsoft.WindowsAppSDK");
    ASSERT_NE(release, nullptr);
    // A type of a package is the restore's to declare; one of Windows is the
    // Windows SDK's, which this profile does not read.
    std::vector<std::string> const parts{
        "Microsoft.WindowsAppSDK " + release->version, utf8(restored_packages_file),
        "Microsoft.UI.Xaml.Controls.NoSuchPanel, listed by " + utf8(profile) +
            ": no package the profiles name declares it",
        "Windows.UI.NoSuchColor, listed by " + utf8(profile) + ": a Windows type",
        "\"windowsMetadata\": true"};
    for (auto&& part : parts) {
        EXPECT_NE(message.find(part), std::string::npos) << part << " in: " << message;
    }
    EXPECT_EQ(message.find("Controls.Button"), std::string::npos) << message;

    std::filesystem::remove_all(profile.parent_path());
}

// An implicit root of the type map that the metadata does not declare stops the
// walk too, named as the type map's.
TEST(crawl, a_root_of_the_type_map_the_metadata_does_not_declare_stops_the_walk) {
    auto const profile =
        write_profile(L"undeclared_root", R"({"Microsoft.UI.Xaml.Controls.Button": {}})");
    auto types = load_type_map(profiles_dir / "types.json");
    types.implicit_roots.insert("Microsoft.UI.Xaml.NoSuchRoot");
    auto const message = walk_error(
        resolve_profiles({profile}, load_restored_packages(restored_packages_file)), types);

    EXPECT_NE(message.find("Microsoft.UI.Xaml.NoSuchRoot, a root of every run in the type map "
                           "(types.json)"),
              std::string::npos)
        << message;
    EXPECT_EQ(message.find("Controls.Button"), std::string::npos) << message;

    std::filesystem::remove_all(profile.parent_path());
}

// A call a profile replaces comes out of the merged profiles under the type and the
// method it was written for, with the function and the header that declares it.
TEST(profile, a_replaced_call_reaches_the_merged_profiles) {
    auto const profiles = resolve_profiles({profiles_dir / "full.winxl-profile"},
                                           load_restored_packages(restored_packages_file));

    auto const type = profiles.replaced_calls.find("Windows.Storage.Streams.RandomAccessStreamReference");
    ASSERT_NE(type, profiles.replaced_calls.end());
    auto const call = type->second.find("CreateFromUri");
    ASSERT_NE(call, type->second.end());
    EXPECT_EQ(call->second.function, "impl::stream_reference_from_uri");
    EXPECT_EQ(call->second.include, "../impl/winrt_helpers.h");
    EXPECT_EQ(type->second.size(), 1u);
}

namespace {

// The members the walk kept, by member name, over every type it reached: what a
// class's interfaces declare is kept on the interface, not on the class. Read
// while the metadata is open, which the types of the closure point into.
std::set<std::string> kept_members(profile_set const& profiles, type_map const& types) {
    std::vector<std::string> files;
    for (auto&& file : profiles.metadata) {
        files.push_back(utf8(file));
    }
    winmd::reader::cache const db{files};
    std::set<std::string> kept;
    for (auto&& [type, members] : crawl(profiles, types, db).members) {
        kept.insert(members.begin(), members.end());
    }
    return kept;
}

}  // namespace

// A class the walk reaches brings in what its binding pairs are made of, though
// the profile keeps none of its members: the property and the events, at the level
// that declares each -- Slider's Value and ValueChanged are RangeBase's, through
// IRangeBase. A class the walk does not reach brings nothing.
TEST(crawl, a_binding_pair_brings_its_members_with_its_class) {
    auto const profile = write_profile(L"pair_members", R"({
        "Microsoft.UI.Xaml.Controls.ToggleSwitch": {"members": []},
        "Microsoft.UI.Xaml.Controls.Slider": {"members": []}
    })", /*windows_metadata=*/true);
    auto types = load_type_map(profiles_dir / "types.json");
    types.binding_pairs = {
        {"Microsoft.UI.Xaml.Controls.ToggleSwitch", "IsOn", {"Toggled"}},
        {"Microsoft.UI.Xaml.Controls.Slider", "Value", {"ValueChanged"}},
        {"Microsoft.UI.Xaml.Controls.ColorPicker", "Color", {"ColorChanged"}},
    };
    auto const kept = kept_members(
        resolve_profiles({profile}, load_restored_packages(restored_packages_file)), types);

    for (auto&& name : {"IsOn", "Toggled", "Value", "ValueChanged"}) {
        EXPECT_TRUE(kept.contains(name)) << name;
    }
    EXPECT_FALSE(kept.contains("ColorChanged"));

    std::filesystem::remove_all(profile.parent_path());
}

// A binding pair the metadata does not back up stops the walk, with the type map and
// the pair named: a class it does not declare, a property or an event neither the
// class nor a class it derives from declares, and a bound member's class.
TEST(crawl, a_binding_pair_the_metadata_does_not_declare_stops_the_walk) {
    auto const profile =
        write_profile(L"undeclared_pair", R"({"Microsoft.UI.Xaml.Controls.Button": {}})");
    auto const profiles = resolve_profiles({profile}, load_restored_packages(restored_packages_file));
    auto const file = profiles_dir / "types.json";

    struct refusal {
        type_map::binding_pair pair;
        std::string says;
    };
    std::vector<refusal> const refusals{
        {{"Microsoft.UI.Xaml.Controls.NoSuchBox", "IsOn", {"Toggled"}},
         "binding pair Microsoft.UI.Xaml.Controls.NoSuchBox.IsOn: the metadata read declares no "
         "class Microsoft.UI.Xaml.Controls.NoSuchBox"},
        {{"Microsoft.UI.Xaml.Controls.ToggleSwitch", "IsLit", {"Toggled"}},
         "binding pair Microsoft.UI.Xaml.Controls.ToggleSwitch.IsLit: "
         "Microsoft.UI.Xaml.Controls.ToggleSwitch declares no property IsLit, and no class it "
         "derives from does"},
        {{"Microsoft.UI.Xaml.Controls.ToggleSwitch", "IsOn", {"Checked"}},
         "binding pair Microsoft.UI.Xaml.Controls.ToggleSwitch.IsOn: "
         "Microsoft.UI.Xaml.Controls.ToggleSwitch declares no event Checked, and no class it "
         "derives from does"},
    };
    for (auto&& [pair, message] : refusals) {
        auto types = load_type_map(file);
        types.binding_pairs = {pair};
        auto const error = walk_error(profiles, types);
        EXPECT_NE(error.find(utf8(file)), std::string::npos) << error;
        EXPECT_NE(error.find(message), std::string::npos) << error;
    }

    auto types = load_type_map(file);
    types.bound_members.push_back({"Microsoft.UI.Xaml.Controls.NoSuchBox", "IsLit", "bool", "both",
                                   "impl::f", "../f.h"});
    auto const error = walk_error(profiles, types);
    EXPECT_NE(error.find("bound member Microsoft.UI.Xaml.Controls.NoSuchBox.IsLit: the metadata "
                         "read declares no class Microsoft.UI.Xaml.Controls.NoSuchBox"),
              std::string::npos)
        << error;

    std::filesystem::remove_all(profile.parent_path());
}

// A pair serves the classes derived from its own, so a second pair of the same
// property on a derived class -- CheckBox under ToggleButton -- would make a binding of
// it on a CheckBox match two; that stops the walk with both named, and so does one
// property bound twice on one class. Pairs of one property on unrelated classes are
// two lines, and stand.
TEST(crawl, two_pairs_of_one_property_along_one_line_stop_the_walk) {
    auto const profile =
        write_profile(L"pair_on_derived", R"({"Microsoft.UI.Xaml.Controls.Button": {}})");
    auto const profiles = resolve_profiles({profile}, load_restored_packages(restored_packages_file));
    type_map::binding_pair const toggle{"Microsoft.UI.Xaml.Controls.Primitives.ToggleButton",
                                        "IsChecked", {"Checked", "Unchecked"}};

    auto types = load_type_map(profiles_dir / "types.json");
    types.binding_pairs = {
        {"Microsoft.UI.Xaml.Controls.CheckBox", "IsChecked", {"Checked"}},
        toggle,
        {"Microsoft.UI.Xaml.Controls.ToggleMenuFlyoutItem", "IsChecked", {"Click"}},
    };
    auto error = walk_error(profiles, types);
    EXPECT_NE(error.find("binding pair Microsoft.UI.Xaml.Controls.CheckBox.IsChecked and binding "
                         "pair Microsoft.UI.Xaml.Controls.Primitives.ToggleButton.IsChecked: "
                         "Microsoft.UI.Xaml.Controls.CheckBox derives from "
                         "Microsoft.UI.Xaml.Controls.Primitives.ToggleButton"),
              std::string::npos)
        << error;
    EXPECT_EQ(error.find("ToggleMenuFlyoutItem"), std::string::npos) << error;

    types.binding_pairs = {toggle, toggle};
    error = walk_error(profiles, types);
    EXPECT_NE(error.find("one property of one class is bound twice"), std::string::npos) << error;

    std::filesystem::remove_all(profile.parent_path());
}
