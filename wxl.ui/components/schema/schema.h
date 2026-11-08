#pragma once

// The whole schema: what the generator writes for the classes it generates,
// and beside it the classes written by hand that take tags in their braces.
// Include this one, never wxl/schema.h directly.
//
// It is the same vocabulary as the generated Members.h, reached through the
// class that declares it: `schema::Button::content` beside the bare
// `dsl::content`. For finding a name rather than remembering it --
// `schema::Button::` offers exactly what a Button takes -- and for two things
// the flat form cannot carry: the anchor knows the class it was named
// through, so writing one class's member on another is refused by name, and
// it knows the type *that* class declares the property with, so the braced
// form survives where two classes disagree.
//
// Each struct mirrors its class's own base, and declares only the members
// that class declares itself; everything else arrives by inheritance, exactly
// as it does on the wrapper.
//
// The entries written by hand live with their classes: a component names its
// fragment <name>.schema.h in the manifest key "schema", and the build
// gathers the fragments of the profile's components into schema.components.h.
// Each entry comes with its compile-only line in the component's surface
// fragment, named by the key "surface": no generator writes one for it.

#include <wxl/schema.h>
#include "schema.components.h"
