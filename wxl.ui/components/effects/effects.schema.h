#pragma once

// The schema entries of the effects and the lights, tested in
// effects.surface.cpp.

#include "BevelEffect.h"
#include "Button3DEffect.h"
#include "GaussianBlurEffect.h"
#include "GlassEffect.h"
#include "HaloEffect.h"
#include "MagnifyEffect.h"
#include "RevealEffect.h"

namespace wxl::dsl::schema {

struct HaloEffect {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Color, ::wxl::Color,
                                               ::wxl::HaloEffect>
        color{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::BlurRadius, double,
                                               ::wxl::HaloEffect>
        blurRadius{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Opacity, double,
                                               ::wxl::HaloEffect>
        opacity{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Offset, ::wxl::Vector3,
                                               ::wxl::HaloEffect>
        offset{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ZIndex, int32_t,
                                               ::wxl::HaloEffect>
        zIndex{};
};

struct GaussianBlurEffect {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Color, ::wxl::Color,
                                               ::wxl::GaussianBlurEffect>
        color{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::BlurRadius, double,
                                               ::wxl::GaussianBlurEffect>
        blurRadius{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Opacity, double,
                                               ::wxl::GaussianBlurEffect>
        opacity{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Gamma, double,
                                               ::wxl::GaussianBlurEffect>
        gamma{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ZIndex, int32_t,
                                               ::wxl::GaussianBlurEffect>
        zIndex{};
};

struct GlassEffect {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Color, ::wxl::Color,
                                               ::wxl::GlassEffect>
        color{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::BlurRadius, double,
                                               ::wxl::GlassEffect>
        blurRadius{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Opacity, double,
                                               ::wxl::GlassEffect>
        opacity{};
};

struct Button3DEffect {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Foreground, ::wxl::Color,
                                               ::wxl::Button3DEffect>
        foreground{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Background, ::wxl::Color,
                                               ::wxl::Button3DEffect>
        background{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::SpecularAmount, double,
                                               ::wxl::Button3DEffect>
        specularAmount{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::SpecularShine, double,
                                               ::wxl::Button3DEffect>
        specularShine{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Emboss, double,
                                               ::wxl::Button3DEffect>
        emboss{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Rim, double,
                                               ::wxl::Button3DEffect>
        rim{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Slope, double,
                                               ::wxl::Button3DEffect>
        slope{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Rounded, bool,
                                               ::wxl::Button3DEffect>
        rounded{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Smooth, double,
                                               ::wxl::Button3DEffect>
        smooth{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Rise, double,
                                               ::wxl::Button3DEffect>
        rise{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Travel, double,
                                               ::wxl::Button3DEffect>
        travel{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Shadow, double,
                                               ::wxl::Button3DEffect>
        shadow{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Azimuth, double,
                                               ::wxl::Button3DEffect>
        azimuth{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Elevation, double,
                                               ::wxl::Button3DEffect>
        elevation{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::LampColor, ::wxl::Color,
                                               ::wxl::Button3DEffect>
        lampColor{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::AmbientColor, ::wxl::Color,
                                               ::wxl::Button3DEffect>
        ambientColor{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::CastBlur, double,
                                               ::wxl::Button3DEffect>
        castBlur{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::HoleShadow, double,
                                               ::wxl::Button3DEffect>
        holeShadow{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::HoleSoftness, double,
                                               ::wxl::Button3DEffect>
        holeSoftness{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::PressDivesLamp, bool,
                                               ::wxl::Button3DEffect>
        pressDivesLamp{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::PressFlashes, bool,
                                               ::wxl::Button3DEffect>
        pressFlashes{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::PressMovesLabel, bool,
                                               ::wxl::Button3DEffect>
        pressMovesLabel{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::PressShades, bool,
                                               ::wxl::Button3DEffect>
        pressShades{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::DiveDepth, double,
                                               ::wxl::Button3DEffect>
        diveDepth{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::DiveGain, double,
                                               ::wxl::Button3DEffect>
        diveGain{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ShadeAmount, double,
                                               ::wxl::Button3DEffect>
        shadeAmount{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::LabelShade, double,
                                               ::wxl::Button3DEffect>
        labelShade{};
};

struct BevelEffect {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::StrokeThickness, double,
                                               ::wxl::BevelEffect>
        strokeThickness{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::BlurRadius, double,
                                               ::wxl::BevelEffect>
        blurRadius{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Offset, double,
                                               ::wxl::BevelEffect>
        offset{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Margin, ::wxl::Thickness,
                                               ::wxl::BevelEffect>
        margin{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::CornerRadius,
                                               ::wxl::CornerRadius, ::wxl::BevelEffect>
        cornerRadius{};
};

struct MagnifyEffect {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Scale, ::wxl::Size,
                                               ::wxl::MagnifyEffect>
        scale{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Maximum, double,
                                               ::wxl::MagnifyEffect>
        maximum{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Minimum, double,
                                               ::wxl::MagnifyEffect>
        minimum{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Duration, ::wxl::core::duration,
                                               ::wxl::MagnifyEffect>
        duration{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::DelayTime, ::wxl::core::duration,
                                               ::wxl::MagnifyEffect>
        delayTime{};
};

struct HoverLight {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Size, double, ::wxl::HoverLight>
        size{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::FullSize, double,
                                               ::wxl::HoverLight>
        fullSize{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Height, double,
                                               ::wxl::HoverLight>
        height{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Intensity, double,
                                               ::wxl::HoverLight>
        intensity{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::DiffuseAmount, double,
                                               ::wxl::HoverLight>
        diffuseAmount{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Color, ::wxl::Color,
                                               ::wxl::HoverLight>
        color{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ConstantAttenuation, double,
                                               ::wxl::HoverLight>
        constantAttenuation{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::LinearAttenuation, double,
                                               ::wxl::HoverLight>
        linearAttenuation{};
};

struct BorderLight {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Size, double, ::wxl::BorderLight>
        size{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::FullSize, double,
                                               ::wxl::BorderLight>
        fullSize{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Height, double,
                                               ::wxl::BorderLight>
        height{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Intensity, double,
                                               ::wxl::BorderLight>
        intensity{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::DiffuseAmount, double,
                                               ::wxl::BorderLight>
        diffuseAmount{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Color, ::wxl::Color,
                                               ::wxl::BorderLight>
        color{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ConstantAttenuation, double,
                                               ::wxl::BorderLight>
        constantAttenuation{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::LinearAttenuation, double,
                                               ::wxl::BorderLight>
        linearAttenuation{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::StrokeThickness, double,
                                               ::wxl::BorderLight>
        strokeThickness{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::CornerRadius,
                                               ::wxl::CornerRadius, ::wxl::BorderLight>
        cornerRadius{};
};

struct RevealEffect {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ZIndex, int32_t,
                                               ::wxl::RevealEffect>
        zIndex{};
};

}  // namespace wxl::dsl::schema
