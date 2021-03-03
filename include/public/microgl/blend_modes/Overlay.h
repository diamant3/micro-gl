#pragma once

#include <microgl/blend_mode_base.h>

namespace microgl {
    namespace blendmode {

        template <bool fast=true, bool use_FPU=false>
        class Overlay : public blend_mode_base<Overlay<fast, use_FPU>> {
        public:

            template<uint8_t bits>
            static inline uint blend_Overlay(cuint b, cuint s) {
                constexpr cuint max = (uint(1)<<bits)-1;
                if(fast)
                    return 2 * b < max ? ((2 * b * s) >> bits) : (max - ((2 * (max - b) * (max - s)) >> bits));
                else if(use_FPU)
                    return 2.0f * float(b) < float(max) ? 2.0f * float(b) * float(s)/float(max) :
                                        (float(max) - ((2.0f * (float(max) - float(b)) * (float(max) - float(s)))/float(max)));
                else
                    return 2 * b < max ? ((2 * mc<bits>(b, s))) : (max - 2*mc<bits>(max-b, max-s));
            }

            template<uint8_t R, uint8_t G, uint8_t B>
            static inline void blend(const color_t &b,
                                     const color_t &s,
                                     color_t &output) {

                output.r = blend_Overlay<R>(b.r, s.r);
                output.g = blend_Overlay<G>(b.g, s.g);
                output.b = blend_Overlay<B>(b.b, s.b);
            }

        };

    }

}