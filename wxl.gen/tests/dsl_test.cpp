#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

#include "wxl.gen.h"

// Last of the headers: it imports std and wxl.core, and MSVC takes no standard
// header after that.
#include "test_directory.h"

import std;
import wxl.core;

namespace {

// Members.h written from a vocabulary given here, in a directory of the test's
// own, emptied before and removed after.
class members : public ::testing::Test {
protected:
    void SetUp() override {
        dir_ = std::filesystem::path{wxl::core::test_directory().native()};
        std::filesystem::remove_all(dir_);
        std::filesystem::create_directories(dir_);
    }

    void TearDown() override { std::filesystem::remove_all(dir_); }

    // Read in text mode, as the file was written, so that a line ends in "\n".
    std::string write(gen::dsl const& d) const {
        gen::emitted em{dir_};
        gen::write_dsl({dir_, 1}, d, em);
        std::ifstream file{dir_ / L"Members.h"};
        return {std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
    }

    std::filesystem::path dir_;
};

}  // namespace

// A name every class declares with one type is a tag of that type; a name the
// classes declare differently is a tag over the list of the types, in the order
// of their names, whatever order the classes came in.
TEST_F(members, a_name_declared_with_several_types_takes_the_list) {
    gen::dsl d;
    d.property_value_types["Text"] = {"hstring"};
    d.property_value_types["Opacity"] = {"float", "double"};
    d.property_value_types["Offset"] = {"Vector3", "float", "Vector2", "double"};

    auto const text = write(d);
    EXPECT_NE(text.find("impl::property_tag<PropertyKey::Text, hstring> text;"), std::string::npos);
    EXPECT_NE(text.find("impl::property_tag<PropertyKey::Opacity, impl::one_of<double, float>> opacity;"),
              std::string::npos);
    EXPECT_NE(text.find("impl::property_tag<PropertyKey::Offset, "
                        "impl::one_of<Vector2, Vector3, double, float>> offset;"),
              std::string::npos);
    EXPECT_EQ(text.find("void>"), std::string::npos);
}

// A brush takes a colour already, so a Color beside a Brush leaves the list; a
// list left with one type is the tag of that type.
TEST_F(members, a_colour_beside_a_brush_leaves_the_list) {
    gen::dsl d;
    d.property_value_types["Background"] = {"Brush", "Color"};
    d.property_value_types["Foreground"] = {"Brush", "Color", "double"};

    auto const text = write(d);
    EXPECT_NE(text.find("impl::property_tag<PropertyKey::Background, Brush> background;"),
              std::string::npos);
    EXPECT_NE(text.find("impl::property_tag<PropertyKey::Foreground, impl::one_of<Brush, double>> "
                        "foreground;"),
              std::string::npos);
}

// A name valued with a class is a tag like any other: the class where every
// declaration agrees on it, the list where they do not.
TEST_F(members, a_class_valued_name_takes_its_class_or_the_list) {
    gen::dsl d;
    d.property_value_types["TitleBar"] = {"UIElement"};
    d.property_value_types["Header"] = {"TitleBar", "Object"};

    auto const text = write(d);
    EXPECT_NE(text.find("impl::property_tag<PropertyKey::TitleBar, UIElement> titleBar;"),
              std::string::npos);
    EXPECT_NE(text.find("impl::property_tag<PropertyKey::Header, impl::one_of<Object, TitleBar>> header;"),
              std::string::npos);
}

// A list over several enumerations has its values written once, ahead of the
// tags: a name one of them has is that one's value, a name several share is one
// shared_enum_value. A list over one enumeration has nothing written: it takes
// the values of that one.
TEST_F(members, the_values_of_a_list_of_enumerations_are_written_once) {
    gen::dsl d;
    d.property_value_types["Placement"] = {"FlyoutPlacementMode", "PlacementMode", "hstring"};
    d.property_value_types["Direction"] = {"AnimationDirection", "Vector3"};
    d.enumerators["FlyoutPlacementMode"] = {{"top", "Top"}, {"full", "Full"}};
    d.enumerators["PlacementMode"] = {{"top", "Top"}, {"mouse", "Mouse"}};
    d.enumerators["AnimationDirection"] = {{"normal", "Normal"}};

    auto const text = write(d);
    auto const values =
        text.find("struct enum_values<one_of<FlyoutPlacementMode, PlacementMode, hstring>> {\n"
                  "    static constexpr FlyoutPlacementMode full = FlyoutPlacementMode::Full;\n"
                  "    static constexpr PlacementMode mouse = PlacementMode::Mouse;\n"
                  "    static constexpr shared_enum_value<FlyoutPlacementMode::Top, PlacementMode::Top> "
                  "top{};\n"
                  "};\n");
    auto const tag = text.find("impl::property_tag<PropertyKey::Placement, "
                               "impl::one_of<FlyoutPlacementMode, PlacementMode, hstring>> placement;");
    ASSERT_NE(values, std::string::npos);
    ASSERT_NE(tag, std::string::npos);
    EXPECT_LT(values, tag);
    EXPECT_EQ(text.find("struct enum_values<one_of<AnimationDirection"), std::string::npos);
}
