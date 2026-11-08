#pragma once

// The schema entries of the hand-written controls, tested in
// controls.surface.cpp.

#include "HeaderedContentControl.h"
#include "SettingsCard.h"
#include "SettingsExpander.h"
#include <wxl/schema.h>

namespace wxl::dsl::schema {

struct SettingsCard : ::wxl::dsl::schema::ContentControl {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Header, ::wxl::Object,
                                               ::wxl::SettingsCard>
        header{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Description, ::wxl::hstring,
                                               ::wxl::SettingsCard>
        description{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::HeaderIcon, ::wxl::IconElement,
                                               ::wxl::SettingsCard>
        headerIcon{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ActionIcon, ::wxl::IconElement,
                                               ::wxl::SettingsCard>
        actionIcon{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::IsClickEnabled, bool,
                                               ::wxl::SettingsCard>
        isClickEnabled{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::IsActionIconVisible, bool,
                                               ::wxl::SettingsCard>
        isActionIconVisible{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::ContentAlignment,
                                               ::wxl::SettingsCardContentAlignment,
                                               ::wxl::SettingsCard>
        contentAlignment{};
};

struct SettingsExpander : ::wxl::dsl::schema::ContentControl {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Header, ::wxl::Object,
                                               ::wxl::SettingsExpander>
        header{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Description, ::wxl::hstring,
                                               ::wxl::SettingsExpander>
        description{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::HeaderIcon, ::wxl::IconElement,
                                               ::wxl::SettingsExpander>
        headerIcon{};
    static constexpr ::wxl::impl::collection_tag<::wxl::PropertyKey::Items, ::wxl::SettingsExpander>
        items{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::IsExpanded, bool,
                                               ::wxl::SettingsExpander>
        isExpanded{};
};

struct HeaderedContentControl : ::wxl::dsl::schema::ContentControl {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::Header, ::wxl::Object,
                                               ::wxl::HeaderedContentControl>
        header{};
};

}  // namespace wxl::dsl::schema
