#pragma once

#include "BPP_RGBA.h"

namespace microgl {
    namespace coder {
        template <typename rgba_>
        using BPP_1_RGBA = BPP_RGBA<1, rgba_>;
    }
}