// Compile-time check of the declarative surface of the effects where it does
// not follow from their descriptions (component.json): the braces, the bare
// forms, the braced anchors. Built into the surface test of every profile that
// selects the component; nothing here runs.

#include "CompositionWindow.h"
#include "FocusEffect.h"
#include "ui.h"
#include "schema.h"

namespace {

using namespace wxl;
using namespace wxl::dsl;

// FocusEffect: attached to the window like ZoomEffect, the look as the bare
// argument, an element that shows its own focus left out.
[[maybe_unused]] void focus_effect(UIElement const& tree) {
    auto const focus = FocusEffect{FocusLook::Lift};
    CompositionWindow const window{focus};
    focus.look(FocusLook::Glow);
    FocusEffect::exclude(tree);
}

// The braces as a text block writes them: the generic tags, a bare colour,
// and both effects attached to a TextBlock, a halo under the glyphs and a glow
// over them.
[[maybe_unused]] void glows_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    HaloEffect const halo{color = rgb(92, 138, 32), blurRadius = 14.0f};
    HaloEffect const bare{rgb(92, 138, 32)};
    GaussianBlurEffect const glow{rgb(0, 0, 0), blurRadius = 3.0f, gamma = 0.5f, opacity = 0.9f,
                                  zIndex = 1};
    TextBlock{u"0", halo, bare, glow};
}

// The pane under a panel, with a bare tint and the tags.
[[maybe_unused]] void glass_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    GlassEffect const pane{rgba(255, 255, 255, 0.2), blurRadius = 24.0f, opacity = 0.9f};
    Border{pane, GlassEffect{blurRadius = 12.0f}};
}

// The braces as a button writes them: the generic tags, a colour where the
// foreground is a brush elsewhere, and the effect attached to a Button.
[[maybe_unused]] void Button3DEffect_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    Button3DEffect const key{foreground = rgb(237, 239, 242), background = rgba(68, 72, 79, 0.933),
                             shadow = 0.7, emboss = -1};
    Button{u"7", key};
    // The lights over the pointer, written bare: one pair given to two kinds
    // of key, and one light written in place.
    HoverLight const spot{size = 200.0f, fullSize = 100.0f, diffuseAmount = 0.4f};
    BorderLight const glow{size = 340.0f, fullSize = 200.0f};
    Button3DEffect const lit{background = rgb(68, 72, 79), spot, glow};
    Button3DEffect const other{background = rgb(48, 68, 116), spot, glow};
    Button3DEffect const dark{background = rgb(48, 68, 116), HoverLight{intensity = 0.0f}};
    Button{u"8", lit};
    Button{u"+", other};
    Button{u"-", dark};
}

[[maybe_unused]] void BevelEffect_margin_braced(::wxl::BevelEffect const& object) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BevelEffect::margin = {1, 2});
}
// The bare forms the braces take: two colours, lit side first, a tagged
// margin and a corner radius by its own type.
[[maybe_unused]] void BevelEffect_bare(::wxl::BevelEffect const& object) {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    BevelEffect const raised{rgba(255, 255, 255, 0.27),
                             rgba(0, 0, 0, 0.38),
                             strokeThickness = 2.0f,
                             blurRadius = 1.0f,
                             offset = 2.0f,
                             Margin{1},
                             CornerRadius{6}};
    Button const key{u"=", raised};
    (void)object;
    (void)key;
}

[[maybe_unused]] void MagnifyEffect_scale_braced(::wxl::MagnifyEffect const& object) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::MagnifyEffect::scale = {1.3f, 1.1f});
}
// What grows is positional, among the scale and the tags.
[[maybe_unused]] void MagnifyEffect_target_positional() {
    using namespace ::wxl::dsl;
    ::wxl::MagnifyEffect const content{1.2f, ::wxl::MagnifyTarget::Content, maximum = 1.35f};
    ::wxl::MagnifyEffect const alone{::wxl::MagnifyTarget::Content};
    ::wxl::Button const button{u"Play", content};
    (void)alone;
    (void)button;
}

// The braces as they are written: the generic tags on each light, whole
// numbers where the setter takes a float, a corner radius by its own type,
// the lights bare in the effect and the effect attached to a Button.
[[maybe_unused]] void RevealEffect_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    HoverLight const hover{size = 140.0f, height = 120.0f, intensity = 1.0f, diffuseAmount = 0.69f};
    BorderLight const border{size = 340.0f,
                             height = 120.0f,
                             intensity = 1.0f,
                             diffuseAmount = 1.24f,
                             strokeThickness = 1.5f,
                             color = rgb(255, 255, 255),
                             constantAttenuation = 1.0f,
                             linearAttenuation = 0.0f,
                             CornerRadius{4}};
    RevealEffect const reveal{hover, border, zIndex = 1};
    Button{u"7", reveal};
    Border{RevealEffect{HoverLight{size = 90.0f}}};
}

}  // namespace
