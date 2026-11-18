#pragma once

// The whole schema, as the generator writes it: the classes it generates, and
// the classes written by hand that the profile's components describe
// (component.json). Include this one, never wxl/schema.h directly.
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

#include <wxl/schema.h>
