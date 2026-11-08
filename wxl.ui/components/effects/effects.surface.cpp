// Compile-time check of the declarative surface of the effects, with the
// lines of their schema entries (effects.schema.h).
// Built into the surface test of every profile that selects the component;
// nothing here runs.

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

// HaloEffect and GaussianBlurEffect -- written by hand, so their schema
// lines are too.
[[maybe_unused]] void HaloEffect_color_assigned(::wxl::HaloEffect const& object,
                                                ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HaloEffect::color = value);
}
[[maybe_unused]] void HaloEffect_blurRadius_assigned(::wxl::HaloEffect const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HaloEffect::blurRadius = value);
}
[[maybe_unused]] void HaloEffect_opacity_assigned(::wxl::HaloEffect const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HaloEffect::opacity = value);
}
[[maybe_unused]] void HaloEffect_offset_assigned(::wxl::HaloEffect const& object,
                                                 ::wxl::Vector3 value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HaloEffect::offset = value);
}
[[maybe_unused]] void HaloEffect_zIndex_assigned(::wxl::HaloEffect const& object, int32_t value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HaloEffect::zIndex = value);
}
[[maybe_unused]] void GaussianBlurEffect_color_assigned(::wxl::GaussianBlurEffect const& object,
                                                        ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GaussianBlurEffect::color = value);
}
[[maybe_unused]] void GaussianBlurEffect_blurRadius_assigned(
    ::wxl::GaussianBlurEffect const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GaussianBlurEffect::blurRadius = value);
}
[[maybe_unused]] void GaussianBlurEffect_opacity_assigned(::wxl::GaussianBlurEffect const& object,
                                                          double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GaussianBlurEffect::opacity = value);
}
[[maybe_unused]] void GaussianBlurEffect_gamma_assigned(::wxl::GaussianBlurEffect const& object,
                                                        double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GaussianBlurEffect::gamma = value);
}
[[maybe_unused]] void GaussianBlurEffect_zIndex_assigned(::wxl::GaussianBlurEffect const& object,
                                                         int32_t value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GaussianBlurEffect::zIndex = value);
}
// The braces as a text block writes them: the generic tags, a bare colour,
// and both effects attached to a TextBlock, a halo under the glyphs and a glow
// over them.
[[maybe_unused]] void glows_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    HaloEffect const halo{color = rgb(92, 138, 32), blurRadius = 14.0f};
    HaloEffect const bare{rgb(92, 138, 32)};
    GaussianBlurEffect const glow{rgb(0, 0, 0), blurRadius = 3.0f, gamma = 0.5, opacity = 0.9,
                                  zIndex = 1};
    TextBlock{u"0", halo, bare, glow};
}

// GlassEffect -- written by hand, so its schema lines are too.
[[maybe_unused]] void GlassEffect_color_assigned(::wxl::GlassEffect const& object,
                                                 ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GlassEffect::color = value);
}
[[maybe_unused]] void GlassEffect_blurRadius_assigned(::wxl::GlassEffect const& object,
                                                      double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GlassEffect::blurRadius = value);
}
[[maybe_unused]] void GlassEffect_opacity_assigned(::wxl::GlassEffect const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::GlassEffect::opacity = value);
}
// The pane under a panel, with a bare tint and the tags.
[[maybe_unused]] void glass_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    GlassEffect const pane{rgba(255, 255, 255, 0.2), blurRadius = 24.0f, opacity = 0.9};
    Border{pane, GlassEffect{blurRadius = 12.0f}};
}

// Button3DEffect -- written by hand, so its schema lines are too.
[[maybe_unused]] void Button3DEffect_foreground_assigned(::wxl::Button3DEffect const& object,
                                                         ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::foreground = value);
}
[[maybe_unused]] void Button3DEffect_background_assigned(::wxl::Button3DEffect const& object,
                                                         ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::background = value);
}
[[maybe_unused]] void Button3DEffect_shadow_assigned(::wxl::Button3DEffect const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::shadow = value);
}
[[maybe_unused]] void Button3DEffect_emboss_assigned(::wxl::Button3DEffect const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::emboss = value);
}
[[maybe_unused]] void Button3DEffect_specularAmount_assigned(::wxl::Button3DEffect const& object,
                                                             double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::specularAmount = value);
}
[[maybe_unused]] void Button3DEffect_specularShine_assigned(::wxl::Button3DEffect const& object,
                                                            double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::specularShine = value);
}
[[maybe_unused]] void Button3DEffect_rim_assigned(::wxl::Button3DEffect const& object,
                                                  double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::rim = value);
}
[[maybe_unused]] void Button3DEffect_slope_assigned(::wxl::Button3DEffect const& object,
                                                    double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::slope = value);
}
[[maybe_unused]] void Button3DEffect_rounded_assigned(::wxl::Button3DEffect const& object,
                                                      bool value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::rounded = value);
}
[[maybe_unused]] void Button3DEffect_smooth_assigned(::wxl::Button3DEffect const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::smooth = value);
}
[[maybe_unused]] void Button3DEffect_rise_assigned(::wxl::Button3DEffect const& object,
                                                   double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::rise = value);
}
[[maybe_unused]] void Button3DEffect_travel_assigned(::wxl::Button3DEffect const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::travel = value);
}
[[maybe_unused]] void Button3DEffect_azimuth_assigned(::wxl::Button3DEffect const& object,
                                                      double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::azimuth = value);
}
[[maybe_unused]] void Button3DEffect_elevation_assigned(::wxl::Button3DEffect const& object,
                                                        double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::elevation = value);
}
[[maybe_unused]] void Button3DEffect_lampColor_assigned(::wxl::Button3DEffect const& object,
                                                        ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::lampColor = value);
}
[[maybe_unused]] void Button3DEffect_ambientColor_assigned(::wxl::Button3DEffect const& object,
                                                           ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::ambientColor = value);
}
[[maybe_unused]] void Button3DEffect_castBlur_assigned(::wxl::Button3DEffect const& object,
                                                       double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::castBlur = value);
}
[[maybe_unused]] void Button3DEffect_holeShadow_assigned(::wxl::Button3DEffect const& object,
                                                         double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::holeShadow = value);
}
[[maybe_unused]] void Button3DEffect_holeSoftness_assigned(::wxl::Button3DEffect const& object,
                                                           double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::holeSoftness = value);
}
[[maybe_unused]] void Button3DEffect_pressDivesLamp_assigned(::wxl::Button3DEffect const& object,
                                                             bool value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::pressDivesLamp = value);
}
[[maybe_unused]] void Button3DEffect_pressFlashes_assigned(::wxl::Button3DEffect const& object,
                                                           bool value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::pressFlashes = value);
}
[[maybe_unused]] void Button3DEffect_pressMovesLabel_assigned(::wxl::Button3DEffect const& object,
                                                              bool value) {
    ::wxl::impl::apply_argument(object,
                                ::wxl::dsl::schema::Button3DEffect::pressMovesLabel = value);
}
[[maybe_unused]] void Button3DEffect_pressShades_assigned(::wxl::Button3DEffect const& object,
                                                          bool value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::pressShades = value);
}
[[maybe_unused]] void Button3DEffect_diveDepth_assigned(::wxl::Button3DEffect const& object,
                                                        double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::diveDepth = value);
}
[[maybe_unused]] void Button3DEffect_diveGain_assigned(::wxl::Button3DEffect const& object,
                                                       double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::diveGain = value);
}
[[maybe_unused]] void Button3DEffect_shadeAmount_assigned(::wxl::Button3DEffect const& object,
                                                          double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::shadeAmount = value);
}
[[maybe_unused]] void Button3DEffect_labelShade_assigned(::wxl::Button3DEffect const& object,
                                                         double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::Button3DEffect::labelShade = value);
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
    HoverLight const spot{size = 200, fullSize = 100, diffuseAmount = 0.4};
    BorderLight const glow{size = 340, fullSize = 200};
    Button3DEffect const lit{background = rgb(68, 72, 79), spot, glow};
    Button3DEffect const other{background = rgb(48, 68, 116), spot, glow};
    Button3DEffect const dark{background = rgb(48, 68, 116), HoverLight{intensity = 0}};
    Button{u"8", lit};
    Button{u"+", other};
    Button{u"-", dark};
}

// BevelEffect -- written by hand, so its schema lines are too.
[[maybe_unused]] void BevelEffect_strokeThickness_assigned(::wxl::BevelEffect const& object,
                                                           double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BevelEffect::strokeThickness = value);
}
[[maybe_unused]] void BevelEffect_blurRadius_assigned(::wxl::BevelEffect const& object,
                                                      double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BevelEffect::blurRadius = value);
}
[[maybe_unused]] void BevelEffect_offset_assigned(::wxl::BevelEffect const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BevelEffect::offset = value);
}
[[maybe_unused]] void BevelEffect_margin_assigned(::wxl::BevelEffect const& object,
                                                  ::wxl::Thickness value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BevelEffect::margin = value);
}
[[maybe_unused]] void BevelEffect_margin_braced(::wxl::BevelEffect const& object) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BevelEffect::margin = {1, 2});
}
[[maybe_unused]] void BevelEffect_cornerRadius_assigned(::wxl::BevelEffect const& object,
                                                        ::wxl::CornerRadius value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BevelEffect::cornerRadius = value);
}
// The bare forms the braces take: two colours, lit side first, a tagged
// margin and a corner radius by its own type.
[[maybe_unused]] void BevelEffect_bare(::wxl::BevelEffect const& object) {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    BevelEffect const raised{rgba(255, 255, 255, 0.27),
                             rgba(0, 0, 0, 0.38),
                             strokeThickness = 2,
                             blurRadius = 1,
                             offset = 2,
                             Margin{1},
                             CornerRadius{6}};
    Button const key{u"=", raised};
    (void)object;
    (void)key;
}

// MagnifyEffect -- written by hand, so its schema lines are too.
[[maybe_unused]] void MagnifyEffect_scale_assigned(::wxl::MagnifyEffect const& object,
                                                   ::wxl::Size value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::MagnifyEffect::scale = value);
}
[[maybe_unused]] void MagnifyEffect_scale_braced(::wxl::MagnifyEffect const& object) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::MagnifyEffect::scale = {1.3, 1.1});
}
[[maybe_unused]] void MagnifyEffect_maximum_assigned(::wxl::MagnifyEffect const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::MagnifyEffect::maximum = value);
}
[[maybe_unused]] void MagnifyEffect_minimum_assigned(::wxl::MagnifyEffect const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::MagnifyEffect::minimum = value);
}
[[maybe_unused]] void MagnifyEffect_duration_assigned(::wxl::MagnifyEffect const& object,
                                                      ::wxl::core::duration value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::MagnifyEffect::duration = value);
}
[[maybe_unused]] void MagnifyEffect_delayTime_assigned(::wxl::MagnifyEffect const& object,
                                                       ::wxl::core::duration value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::MagnifyEffect::delayTime = value);
}
// What grows is positional, among the scale and the tags.
[[maybe_unused]] void MagnifyEffect_target_positional() {
    using namespace ::wxl::dsl;
    ::wxl::MagnifyEffect const content{1.2, ::wxl::MagnifyTarget::Content, maximum = 1.35};
    ::wxl::MagnifyEffect const alone{::wxl::MagnifyTarget::Content};
    ::wxl::Button const button{u"Play", content};
    (void)alone;
    (void)button;
}

// RevealEffect and its two lights -- written by hand, so their schema lines are too.
[[maybe_unused]] void HoverLight_size_assigned(::wxl::HoverLight const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HoverLight::size = value);
}
[[maybe_unused]] void HoverLight_height_assigned(::wxl::HoverLight const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HoverLight::height = value);
}
[[maybe_unused]] void HoverLight_intensity_assigned(::wxl::HoverLight const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HoverLight::intensity = value);
}
[[maybe_unused]] void HoverLight_diffuseAmount_assigned(::wxl::HoverLight const& object,
                                                        double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HoverLight::diffuseAmount = value);
}
[[maybe_unused]] void HoverLight_color_assigned(::wxl::HoverLight const& object,
                                                ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HoverLight::color = value);
}
[[maybe_unused]] void HoverLight_constantAttenuation_assigned(::wxl::HoverLight const& object,
                                                              double value) {
    ::wxl::impl::apply_argument(object,
                                ::wxl::dsl::schema::HoverLight::constantAttenuation = value);
}
[[maybe_unused]] void HoverLight_linearAttenuation_assigned(::wxl::HoverLight const& object,
                                                            double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::HoverLight::linearAttenuation = value);
}
[[maybe_unused]] void BorderLight_size_assigned(::wxl::BorderLight const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::size = value);
}
[[maybe_unused]] void BorderLight_height_assigned(::wxl::BorderLight const& object, double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::height = value);
}
[[maybe_unused]] void BorderLight_intensity_assigned(::wxl::BorderLight const& object,
                                                     double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::intensity = value);
}
[[maybe_unused]] void BorderLight_diffuseAmount_assigned(::wxl::BorderLight const& object,
                                                         double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::diffuseAmount = value);
}
[[maybe_unused]] void BorderLight_color_assigned(::wxl::BorderLight const& object,
                                                 ::wxl::Color value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::color = value);
}
[[maybe_unused]] void BorderLight_constantAttenuation_assigned(::wxl::BorderLight const& object,
                                                               double value) {
    ::wxl::impl::apply_argument(object,
                                ::wxl::dsl::schema::BorderLight::constantAttenuation = value);
}
[[maybe_unused]] void BorderLight_linearAttenuation_assigned(::wxl::BorderLight const& object,
                                                             double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::linearAttenuation = value);
}
[[maybe_unused]] void BorderLight_strokeThickness_assigned(::wxl::BorderLight const& object,
                                                           double value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::strokeThickness = value);
}
[[maybe_unused]] void BorderLight_cornerRadius_assigned(::wxl::BorderLight const& object,
                                                        ::wxl::CornerRadius value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::BorderLight::cornerRadius = value);
}
[[maybe_unused]] void RevealEffect_zIndex_assigned(::wxl::RevealEffect const& object,
                                                   int32_t value) {
    ::wxl::impl::apply_argument(object, ::wxl::dsl::schema::RevealEffect::zIndex = value);
}
// The braces as they are written: the generic tags on each light, whole
// numbers where the setter takes a double, a corner radius by its own type,
// the lights bare in the effect and the effect attached to a Button.
[[maybe_unused]] void RevealEffect_in_braces() {
    using namespace ::wxl;
    using namespace ::wxl::dsl;
    HoverLight const hover{size = 140, height = 120, intensity = 1.0, diffuseAmount = 0.69};
    BorderLight const border{size = 340,
                             height = 120,
                             intensity = 1.0,
                             diffuseAmount = 1.24,
                             strokeThickness = 1.5,
                             color = rgb(255, 255, 255),
                             constantAttenuation = 1.0,
                             linearAttenuation = 0.0,
                             CornerRadius{4}};
    RevealEffect const reveal{hover, border, zIndex = 1};
    Button{u"7", reveal};
    Border{RevealEffect{HoverLight{size = 90}}};
}

}  // namespace
