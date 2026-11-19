#include <format>
#include <ostream>
#include <print>

#include "wxl.gen.h"
import std;
import wxl.core;
// Members.h -- the surface the builder syntax is written against.
//
// Two things per member, and nothing per class. A tag object (`content`,
// `onClick`) is what the DSL writes on the left of the `=`; behind it, one
// property_setter / event_adder specialisation says which member of the
// object the assignment reaches. Both halves of the dispatch are templates
// on the object type, so a single specialisation serves every class that
// declares that member -- which is the whole reason the key enums are flat
// rather than per class.
//
// Only names that came out of the profile-driven closure are emitted, so a
// narrow profile yields a correspondingly narrow DSL surface. The tags are
// `inline constexpr` empty objects: they cost nothing at runtime and exist
// purely to give the assignment a left-hand side.
namespace gen {
namespace {

// The values of every enumeration a property has, each written once, in
// namespace impl. A tag over the enumeration inherits them
// (impl::enum_values), the flat vocabulary and the schema anchors alike, so
// `orientation.horizontal` needs no tag type of its own.
void write_enum_values(std::ostream& file, dsl const& d) {
    for (auto&& [type, values] : d.enumerators) {
        if (values.empty()) {
            continue;
        }
        std::print(file, "\ntemplate <>\nstruct enum_values<{}> {{\n", type);
        for (auto&& [member, enumerator] : values) {
            std::print(file, "    static constexpr {0} {1} = {0}::{2};\n", type, member,
                       enumerator);
        }
        std::print(file, "}};\n");
    }
}

// The types a tag takes, in the order of the set, which is what keeps the
// output the same from run to run. A Brush takes a Color already -- the solid
// brush of that colour (impl::solid_color_brush), which is still the colour for
// a setter that takes one -- so beside a Brush a Color would be a second
// operator= over the same argument.
std::vector<std::string> tag_types(std::set<std::string> const& declared) {
    std::vector<std::string> types;
    for (auto&& type : declared) {
        if (type == "Color" && declared.contains("Brush")) {
            continue;
        }
        types.push_back(type);
    }
    return types;
}

// The types of a tag as its template argument: the one type, or impl::one_of
// over the list. `one_of` is spelled as the namespace it is written in sees it.
std::string tag_value(std::vector<std::string> const& types, std::string_view one_of) {
    if (types.size() == 1) {
        return types.front();
    }
    std::string list;
    for (auto&& type : types) {
        list += list.empty() ? type : ", " + type;
    }
    return std::format("{}<{}>", one_of, list);
}

// The values of each list over several enumerations, in namespace impl, after
// the values of the enumerations themselves and ahead of the first tag over the
// list. Each value name is written once: a name one enumeration of the list has
// is that enumeration's value, a name several share is one
// impl::shared_enum_value, which the setter of the object takes as its own
// enumeration. A list over one enumeration needs nothing here -- it inherits the
// values of that one (impl::enum_values<one_of<...>>).
void write_list_enum_values(std::ostream& file, dsl const& d) {
    auto const has_values = [&d](std::string const& type) {
        auto const values = d.enumerators.find(type);
        return values != d.enumerators.end() && !values->second.empty();
    };
    std::set<std::vector<std::string>> lists;
    for (auto&& [name, declared] : d.property_value_types) {
        auto types = tag_types(declared);
        if (std::ranges::count_if(types, has_values) > 1) {
            lists.insert(std::move(types));
        }
    }

    for (auto&& types : lists) {
        // Value name -> the enumeration and the enumerator of each that has it.
        std::map<std::string, std::vector<std::pair<std::string, std::string>>> values;
        for (auto&& type : types) {
            if (has_values(type)) {
                for (auto&& [member, enumerator] : d.enumerators.at(type)) {
                    values[member].emplace_back(type, enumerator);
                }
            }
        }
        std::print(file, "\ntemplate <>\nstruct enum_values<{}> {{\n", tag_value(types, "one_of"));
        for (auto&& [member, spellings] : values) {
            if (spellings.size() == 1) {
                std::print(file, "    static constexpr {0} {1} = {0}::{2};\n", spellings.front().first,
                           member, spellings.front().second);
                continue;
            }
            std::string shared;
            for (auto&& [type, enumerator] : spellings) {
                shared += std::format("{}{}::{}", shared.empty() ? "" : ", ", type, enumerator);
            }
            std::print(file, "    static constexpr shared_enum_value<{}> {}{{}};\n", shared, member);
        }
        std::print(file, "}};\n");
    }
}

// The dispatch behind the tags, in namespace impl: one specialisation per
// key.
void write_dispatch(std::ostream& file, dsl const& d) {
    // The key enums keep the metadata name; everything the DSL and the
    // wrappers spell is the member name, camelCase.
    //
    // Constrained on the call it makes, so that a class without the setter
    // refuses the assignment at the tag rather than inside this body -- and
    // so that a binding can ask, before it commits to writing, whether there
    // is anything to write to.
    for (auto&& [name, value_types] : d.property_value_types) {
        std::print(file, R"(
template <>
struct property_setter<PropertyKey::{0}> {{
    template <typename Obj, typename T>
        requires requires(Obj const& object, T const& value) {{ object.{1}(value); }}
    static void set(Obj const& object, T const& value) {{
        object.{1}(value);
    }}
}};
)",
                   name, member_name(name));
    }

    // An attached property's setter belongs to another class entirely: the
    // tag is written inside the child, and the call goes to the statics of
    // the parent whose layout the value is for. Unless the object has a
    // member of that name itself: a hand-written class may take the same
    // tag for a property of its own -- zIndex on an effect, saying where its
    // layer lies -- and then the tag is the object's.
    //
    // When several parents declare the name, the child cannot know which one
    // it will be put into, so every setter the object fits is called and the
    // others are skipped; the object has to fit at least one.
    for (auto&& [name, owners] : d.attached) {
        std::string calls;
        if (owners.size() == 1) {
            calls = std::format("            {}::{}(object, value);\n", owners.front().owner,
                                owners.front().setter);
        } else {
            std::string fits;
            for (auto&& attached : owners) {
                calls += std::format(
                    "            if constexpr (requires {{ {0}::{1}(object, value); }}) {{\n"
                    "                {0}::{1}(object, value);\n"
                    "            }}\n",
                    attached.owner, attached.setter);
                fits += std::format("{}requires {{ {}::{}(object, value); }}",
                                    fits.empty() ? "" : " || ", attached.owner, attached.setter);
            }
            calls += std::format(
                "            static_assert({}, \"wxl: no parent of the attached property {} takes this object\");\n",
                fits, name);
        }
        std::print(file, R"(
template <>
struct property_setter<PropertyKey::{0}> {{
    template <typename Obj, typename T>
    static void set(Obj const& object, T const& value) {{
        if constexpr (requires {{ object.{1}(value); }}) {{
            object.{1}(value);
        }} else {{
{2}        }}
    }}
}};
)",
                   name, member_name(name), calls);
    }

    // A collection-valued property is filled, not assigned: the setter is
    // handed the whole run of items, so it can reach the collection once and
    // decide for itself how to put them in.
    for (auto&& [name, element] : d.collection_element) {
        std::print(file, R"(
template <>
struct collection_setter<PropertyKey::{0}> {{
    template <typename Obj, typename... Items>
    static void set(Obj const& object, Items const&... items) {{
        auto const collection = object.{1}();
        (collection.append(items), ...);
    }}
}};
)",
                   name, member_name(name));
    }

    // remove passes on whatever the class's remove_on... answers: nothing from a generated
    // wrapper, whose WinRT event does not say, and whether the token was there from
    // CompositionWindow, whose WindowWatch checks it. The token is the caller's own, by
    // reference: remove_on... empties it.
    for (auto&& name : d.events) {
        std::print(file, R"(
template <>
struct event_adder<EventKey::{0}> {{
    template <typename Obj, typename Fn>
    static EventToken add(Obj const& object, Fn const& handler) {{
        return object.add_on{0}(handler);
    }}

    template <typename Obj>
    static auto remove(Obj const& object, EventToken& token) {{
        return object.remove_on{0}(token);
    }}

    template <typename Obj>
    using args_t = event_args_of_t<decltype(&Obj::add_on{0})>;
}};
)",
                   name);
    }
}

// The tag of one property: what its type and the vocabulary around it make
// it.
void write_property_tag(std::ostream& file, dsl const& d, std::string const& name,
                        std::set<std::string> const& declared) {
    auto const types = tag_types(declared);
    auto const value = tag_value(types, "impl::one_of");

    // A collection that a setter can also be given in one go --
    // `rowDefinitions[a, b]` and `rowDefinitions = lengths` are the same
    // property said two ways -- so the tag carries both.
    if (d.collection_element.count(name)) {
        std::print(file, "inline constexpr impl::assignable_collection_tag<PropertyKey::{}, {}> {};\n",
                   name, value, member_name(name));
        return;
    }

    // A property whose declarations disagree on the type takes each of them,
    // impl::one_of over the list. An enum-typed property is written the same
    // way: the values it accepts come with its type (impl::enum_values), so
    // `orientation.horizontal` needs no enum name.
    std::print(file, "inline constexpr impl::property_tag<PropertyKey::{}, {}> {};\n", name, value,
               member_name(name));
}

// The tags, in namespace dsl: properties, attached properties, collections,
// events.
void write_tag_objects(std::ostream& file, dsl const& d) {
    for (auto&& [name, declared] : d.property_value_types) {
        write_property_tag(file, d, name, declared);
    }

    if (!d.attached.empty()) {
        std::print(file, "\n");
        for (auto&& [name, owners] : d.attached) {
            std::print(file, "inline constexpr impl::property_tag<PropertyKey::{}, {}> {};\n", name,
                       owners.front().value_type, member_name(name));
        }
    }

    if (!d.collection_element.empty()) {
        std::print(file, "\n");
        for (auto&& [name, element] : d.collection_element) {
            if (d.property_value_types.count(name)) {
                continue;  // already written above, carrying both forms
            }
            std::print(file, "inline constexpr impl::collection_tag<PropertyKey::{}> {};\n", name,
                       member_name(name));
        }
    }

    std::print(file, "\n");
    for (auto&& name : d.events) {
        std::print(file, "inline constexpr impl::event_tag<EventKey::{}> on{};\n", name, name);
    }
}

}  // namespace

void write_dsl(output const& out, dsl const& d, emitted& em) {
    auto const path = out.dir / "Members.h";
    auto file = open_output(path);
    std::print(file, "{}#pragma once\n\n", banner);
    write_include(file, "../impl/member.h");
    write_include(file, "Tags.h");
    write_includes(file, d.includes);

    // small is a macro of the Windows SDK (rpcndr.h: #define small char), and the value tags of an
    // enum are spelled with the enum's own names -- CompactOverlaySize::Small is the tag
    // small. The tags are spelled below, after whatever Windows header was included first.
    std::print(file, "\n#ifdef small\n#undef small\n#endif\n");

    std::print(file, "\nnamespace wxl {{\n\nnamespace impl {{\n");
    // Ahead of everything below: a specialisation has to precede the first
    // property_tag over its enumeration or its list.
    write_enum_values(file, d);
    write_list_enum_values(file, d);
    write_dispatch(file, d);

    // The tags live in a namespace of their own so that consuming code brings
    // the vocabulary in deliberately rather than having every property name in
    // scope the moment it names a wxl type.
    std::print(file, "\n}}  // namespace impl\n\nnamespace dsl {{\n\n");
    write_tag_objects(file, d);
    std::print(file, "\n}}  // namespace dsl\n}}  // namespace wxl\n");

    em.add(path);
    std::print("generated {} ({} property tags, {} collection tags, {} event tags)\n",
               wxl::core::unicode::to_utf8(path).chars(), d.property_value_types.size(),
               d.collection_element.size(), d.events.size());
}

}  // namespace gen
