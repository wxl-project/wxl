#pragma once

// The schema entry of the available-size layout, tested in
// layout.surface.cpp.

#include "AvailableSizeLayout.h"

namespace wxl::dsl::schema {

struct AvailableSizeLayout {
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::AvailableWidth, double,
                                               ::wxl::AvailableSizeLayout>
        availableWidth{};
    static constexpr ::wxl::impl::property_tag<::wxl::PropertyKey::AvailableHeight, double,
                                               ::wxl::AvailableSizeLayout>
        availableHeight{};
};

}  // namespace wxl::dsl::schema
