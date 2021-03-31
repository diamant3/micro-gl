#pragma once

#include <microgl/pixel_coders/BPP_RGBA.h>

namespace microgl {
    namespace coder {

        template <typename rgba_>
        using BPP_2_RGBA = BPP_RGBA<2, rgba_>;
    }
}