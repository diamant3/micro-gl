#pragma once

namespace microgl {

    typedef struct {
        uint8_t r=0, g=0, b=0, a=0;
        uint8_t r_bits=8, g_bits=8, b_bits=8, a_bits=8;
    } color_t;

    struct color_f_t {
        float r, g, b, a;

//        color_f_t& operator*(const float & val)
//        {
//            r *= val;
//            g *= val;
//            b *= val;
//            return *this;
//        }

    };

    color_f_t RED{1.0,0.0,0.0, 1.0};
    color_f_t YELLOW{1.0,1.0,0.0, 1.0};
    color_f_t WHITE{1.0,1.0,1.0, 1.0};
    color_f_t GREEN{0.0,1.0,0.0, 1.0};
    color_f_t BLUE{0.0,0.0,1.0, 1.0};
    color_f_t BLACK{0.0,0.0,0.0, 1.0};

}
