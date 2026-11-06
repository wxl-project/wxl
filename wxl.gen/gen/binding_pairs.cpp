#include <format>
#include <ostream>
#include <print>

#include "wxl.gen.h"

import std;
import wxl.core;

// BindingPairs.h -- the binding pairs of the classes the run generates: what
// `isOn = Bind{field}` and `isChecked = BindInput{field}` find
// (impl::bind_property in impl/member.h).
//
// Each pair is an explicit specialisation impl::binding_pair<key, owner>, and
// a partial specialisation of impl::property_binder hands it every class
// derived from the owner: a CheckBox binds isChecked through ToggleButton's
// pair, a ComboBox selectedIndex through Selector's. The walk refused two pairs
// of one key along one line of inheritance (crawl.h), so no class matches two
// of these. The owners are declared, not included: the constraint and the
// conversion to the owner are checked where a binding is written, which has
// the control's whole class.
//
// A pair of the metadata declares one bind() per field type its property
// converts to; the bodies are written beside the owner's other bodies
// (gen/classes.cpp), compiled once. A bound member's bind() calls the
// hand-written function the type map names, inline: it is one call to a
// function declared without winrt. So the header names no winrt type, and
// neither does Bind.h, which includes it.

namespace gen {

void write_binding_pairs(output const& out, model const& m, emitted& em) {
    auto const path = out.dir / "BindingPairs.h";
    auto file = open_output(path);
    std::print(file, "{}#pragma once\n\n", banner);

    std::set<std::string> includes{"../impl/member.h"};
    std::set<std::string> owners;
    for (auto&& pair : m.binding_pairs) {
        includes.insert(pair.includes.begin(), pair.includes.end());
        owners.insert(pair.owner);
    }
    write_includes(file, includes);

    std::print(file, "\nnamespace wxl {{\n\n");
    for (auto&& owner : owners) {
        std::print(file, "class {};\n", owner);
    }
    std::print(file, "\nnamespace impl {{\n");

    for (auto&& pair : m.binding_pairs) {
        std::print(file, R"(
template <>
struct binding_pair<PropertyKey::{}, {}> {{
    static constexpr bind_direction direction = bind_direction::{};
)",
                   pair.key, pair.owner, pair.direction);
        for (auto&& field : pair.fields) {
            if (pair.function.empty()) {
                std::print(file,
                           "    static void bind({} const& control, core::observable<{}>& field,\n"
                           "                     bind_direction direction);\n",
                           pair.owner, field.type);
            } else {
                std::print(file,
                           "    static void bind({} const& control, core::observable<{}>& field,\n"
                           "                     bind_direction direction) {{\n"
                           "        {}(control, field, direction);\n"
                           "    }}\n",
                           pair.owner, field.type, pair.function);
            }
        }
        std::print(file, R"(}};

template <class Control>
    requires std::derived_from<Control, {1}>
struct property_binder<PropertyKey::{0}, Control> : binding_pair<PropertyKey::{0}, {1}> {{
}};
)",
                   pair.key, pair.owner);
    }

    std::print(file, "\n}}  // namespace impl\n}}  // namespace wxl\n");
    em.add(path);
    std::print("generated {} ({} binding pairs)\n", wxl::core::unicode::to_utf8(path).chars(),
               m.binding_pairs.size());
}

}  // namespace gen
