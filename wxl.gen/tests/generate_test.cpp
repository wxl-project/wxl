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
