#include "../include/microgl/Canvas.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
template<typename P, typename CODER>
Canvas<P, CODER>::Canvas(Bitmap<P, CODER> *$bmp)
                        : _bitmap_canvas($bmp), _width{$bmp->width()}, _height{$bmp->height()} {

    _flag_hasAlphaChannel = (pixelFormat()==PixelFormat::RGBA8888) ||
                            (pixelFormat()==PixelFormat::RGBA4444) ||
                            (pixelFormat()==PixelFormat::RGBA5551);

}

template<typename P, typename CODER>
Canvas<P, CODER>::Canvas(int width, int height, PixelCoder<P, CODER> * $coder) :
            Canvas<P, CODER>(new Bitmap<P, CODER>(width, height, $coder)) {

}

template<typename P, typename CODER>
inline PixelCoder<P, CODER> *Canvas<P, CODER>::coder() {
    return _bitmap_canvas->coder();
}

template<typename P, typename CODER>
inline Bitmap<P, CODER> *Canvas<P, CODER>::bitmapCanvas() {
    return _bitmap_canvas;
}

template<typename P, typename CODER>
PixelFormat Canvas<P, CODER>::pixelFormat() {
    return coder()->format();
}

template<typename P, typename CODER>
unsigned int Canvas<P, CODER>::sizeofPixel() {
    return sizeof(P{});
}

template<typename P, typename CODER>
P &Canvas<P, CODER>::getPixel(int x, int y) {
    // this is not good for high performance loop, cannot be inlined
    return _bitmap_canvas->readAt(y*_width + x);
}

template<typename P, typename CODER>
P &Canvas<P, CODER>::getPixel(int index) {
    // this is not good for high performance loop, cannot be inlined
    return _bitmap_canvas->readAt(index);
}

template<typename P, typename CODER>
bool Canvas<P, CODER>::hasAlphaChannel() {
    return _flag_hasAlphaChannel;
}

template<typename P, typename CODER>
bool Canvas<P, CODER>::hasAntialiasing() {
    return _flag_antiAlias;
}

template<typename P, typename CODER>
void Canvas<P, CODER>::setAntialiasing(bool value) {
    _flag_antiAlias = value;
}

template<typename P, typename CODER>
void Canvas<P, CODER>::getPixelColor(int x, int y, color_t & output) {
    this->_bitmap_canvas->decode(x, y, output);
}

template<typename P, typename CODER>
void Canvas<P, CODER>::getPixelColor(int index, color_t & output) {
    this->_bitmap_canvas->decode(index, output);
}

template<typename P, typename CODER>
void Canvas<P, CODER>::getPixelColor(int x, int y, color_f_t & output) {
    this->_bitmap_canvas->decode(x, y, output);
}

template<typename P, typename CODER>
void Canvas<P, CODER>::getPixelColor(int index, color_f_t & output) {
    this->_bitmap_canvas->decode(index, output);
}

template<typename P, typename CODER>
int Canvas<P, CODER>::width() {
    return _width;
}

template<typename P, typename CODER>
int Canvas<P, CODER>::height() {
    return _height;
}

template<typename P, typename CODER>
void Canvas<P, CODER>::setBlendMode(const BlendMode &mode) {
    _blend_mode = mode;
}

template<typename P, typename CODER>
void Canvas<P, CODER>::setPorterDuffMode(const PorterDuff &mode) {
    _porter_duff_mode = mode;
}

template<typename P, typename CODER>
BlendMode &Canvas<P, CODER>::getBlendMode() {
    return _blend_mode;
}

template<typename P, typename CODER>
PorterDuff &Canvas<P, CODER>::getPorterDuffMode() {
    return _porter_duff_mode;
}

template<typename P, typename CODER>
P *Canvas<P, CODER>::pixels() {
    return _bitmap_canvas->data();
}

template<typename P, typename CODER>
void Canvas<P, CODER>::clear(const color_f_t &color) {
//    _bitmap_canvas->fill(encodeFloatRGB(color, pixelFormat()));

    P output;
    _bitmap_canvas->coder()->encode(color, output);
    _bitmap_canvas->fill(output);
}

template<typename P, typename CODER>
inline void Canvas<P, CODER>::blendColor(const color_f_t &val, int x, int y) {
    color_f_t result = val;

    if(false && hasAlphaChannel()) {
        color_f_t backdrop;
        getPixelColor(x, y, backdrop);
        const color_f_t & src = val;
        color_f_t blended = blend_mode_apply(_blend_mode, backdrop, src);

        color_f_t blend_in_place{};
        blend_in_place.r = (1.0 - backdrop.a) * src.r + backdrop.a*blended.r;
        blend_in_place.g = (1.0 - backdrop.a) * src.g + backdrop.a*blended.g;
        blend_in_place.b = (1.0 - backdrop.a) * src.b + backdrop.a*blended.b;
        blend_in_place.a = float(src.a);

        result = porter_duff_apply(_porter_duff_mode, backdrop, blend_in_place);
    }

//    int index = (y * width() + x);
    P output;
//    coder()->encode_from_normalized(result, output);
    coder()->encode(result, output);

    drawPixel(output, x, y);
}


template<typename P, typename CODER>
inline void Canvas<P, CODER>::blendColor(const color_f_t &val, int index) {
    color_f_t result = val;

    if(false &&  hasAlphaChannel()) {
        color_f_t backdrop;
        getPixelColor(index, backdrop);
        const color_f_t & src = val;
        color_f_t blended = blend_mode_apply(_blend_mode, backdrop, src);

        color_f_t blend_in_place{};
        blend_in_place.r = (1.0 - backdrop.a) * src.r + backdrop.a*blended.r;
        blend_in_place.g = (1.0 - backdrop.a) * src.g + backdrop.a*blended.g;
        blend_in_place.b = (1.0 - backdrop.a) * src.b + backdrop.a*blended.b;
        blend_in_place.a = float(src.a);

        result = porter_duff_apply(_porter_duff_mode, backdrop, blend_in_place);
    }

    P output{};

    coder()->encode(result, output);

//    _bitmap_canvas->_data[index] = output;
//
    drawPixel(output, index);
}

template<typename P, typename CODER>
void Canvas<P, CODER>::drawPixel(const P & val, int x, int y) {
    int index = (y * width() + x);
    _bitmap_canvas->writeAt(val, index);
}

template<typename P, typename CODER>
inline void Canvas<P, CODER>::drawPixel(const P & val, int index) {
    _bitmap_canvas->writeAt(val, index);
}


template<typename P, typename CODER>
void Canvas<P, CODER>::drawCircle(const color_f_t & color,
                           int centerX, int centerY,
                           int radius) {
    uint8_t nSubpixelsX ,nSubpixelsY;
    color_f_t color_res = color;

    nSubpixelsX = nSubpixelsY = hasAntialiasing() ? 4 : 1;

    int x1 = centerX - radius, y1 = centerY - radius;
    int x2 = centerX + radius, y2 = centerY + radius;
    int index;

    for (int y = y1; y < y2; ++y) {
        // this is an optimization instead of multiplying per pixel
        index = y * _width;
        for (int x = x1; x < x2; ++x) {

            // Compute the coverage by sampling the circle at "subpixel"
            // locations and counting the number of subpixels turned on.
            float coverage = 0.0f;

            for (int subpixelY = 0; subpixelY < nSubpixelsY; subpixelY++) {
                for (int subpixelX = 0; subpixelX < nSubpixelsX; subpixelX++) {
                    // Sample the center of the subpixel.
                    float sampX = x + ((subpixelX + 0.5) / nSubpixelsX);
                    float sampY = y + ((subpixelY + 0.5) / nSubpixelsY);
                    if (insideCircle(sampX, sampY, centerX, centerY, radius))
                        coverage += 1;
                }
            }

            // Take the average of all subpixels.
            coverage /= nSubpixelsX * nSubpixelsY;

            // Quick optimization: if we're fully outside the circle,
            // we don't need to compute the fill.
            if (coverage == 0)
                continue;

            color_res.a = color.a * coverage;
            blendColor(color_res, index + x);

        }

    }

}

template<typename P, typename CODER>
void Canvas<P, CODER>::drawGradient(const color_f_t & startColor,
                             const color_f_t & endColor,
                             int left, int top, int w, int h) {
    float t;
    color_f_t res{};

    for (int x = left; x < left + w; ++x) {

        t = float(x - left) / (w);

        float r = lerp(startColor.r, endColor.r, t);
        float g = lerp(startColor.g, endColor.g, t);
        float b = lerp(startColor.b, endColor.b, t);
        float a = lerp(startColor.a, endColor.a, t);

        res = {r, g, b, a};

        drawQuad(res, x, top, 1, h);
    }

}


#include "../include/microgl/Fixed.h"
typedef int fixed;

int orient2d(const vec2_32i& a, const vec2_32i& b, const vec2_32i& c)
{
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}


template<typename P, typename CODER>
void Canvas<P, CODER>::drawTriangle(const color_f_t &color,
                                    const int v0_x, const int v0_y,
                                    const int v1_x, const int v1_y,
                                    const int v2_x, const int v2_y) {

    // bounding box
    int minX = std::min({v0_x, v1_x, v2_x});
    int minY = std::min({v0_y, v1_y, v2_y});
    int maxX = std::max({v0_x, v1_x, v2_x});
    int maxY = std::max({v0_y, v1_y, v2_y});

    // Triangle setup
    int A01 = v0_y - v1_y, B01 = v1_x - v0_x;
    int A12 = v1_y - v2_y, B12 = v2_x - v1_x;
    int A20 = v2_y - v0_y, B20 = v0_x - v2_x;

    // Barycentric coordinates at minX/minY corner
    vec2_32i p = { minX, minY };

    // overflow safety safe_bits>=(p-2)/2, i.e 15 bits (0..32,768) for 32 bits integers.
    // https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
    // for 16 bits computer this is safe for 7 bits input [0..127] - not good
    int w0_row = orient2d({v1_x, v1_y}, {v2_x, v2_y}, p);
    int w1_row = orient2d({v2_x, v2_y}, {v0_x, v0_y}, p);
    int w2_row = orient2d({v0_x, v0_y}, {v1_x, v1_y}, p);
    int index;

    index = p.y*_width;

    for (p.y = minY; p.y <= maxY; p.y++) {

        // Barycentric coordinates at start of row
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;


        for (p.x = minX; p.x <= maxX; p.x++) {
//            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
            // same as (w0 >= 0 && w1 >= 0 && w2 >= 0), but use only MSB,
            // this turns three conditionals into one !!!
            if ((w0 | w1 | w2) >= 0) {
                blendColor(color, index + p.x);

                // this is faster if we don't use blending
//                P output{};
//                drawPixel(0xff, p.x, p.y);
            }

            // One step to the right
            w0 += A12;
            w1 += A20;
            w2 += A01;

        }

        // One row step
        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
        index += _width;
    }

}


template<typename P, typename CODER>
template<typename P2, typename CODER2>
void
Canvas<P, CODER>::drawTriangle(Bitmap<P2, CODER2> & bmp,
                               int v0_x, int v0_y, float u0, float v0,
                               int v1_x, int v1_y, float u1, float v1,
                               int v2_x, int v2_y, float u2, float v2) {

//    float area = orient2d({v0_x, v0_y}, {v1_x, v1_y}, {v2_x, v2_y});
    float area = orient2d({v0_x, v0_y}, {v1_x, v1_y}, {v2_x, v2_y});
    int bmp_width = bmp.width();
    // bounding box
    int minX = std::min({v0_x, v1_x, v2_x});
    int minY = std::min({v0_y, v1_y, v2_y});
    int maxX = std::max({v0_x, v1_x, v2_x});
    int maxY = std::max({v0_y, v1_y, v2_y});

//    minX = 0; minY = 210;
//    maxX = 200;maxY=260;

    // clipping
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, this->width() - 1);
    maxY = std::min(maxY, this->height() - 1);

    fixed one_over_area = fixed_one_over_int(area);

    // Triangle setup
    fixed A01_u2 = float_to_fixed(u2*bmp.width()*(v0_y - v1_y)/area), B01_u2 = float_to_fixed(u2*bmp.width()*(v1_x - v0_x)/area); //w2
    fixed A12_u0 = float_to_fixed(u0*bmp.width()*(v1_y - v2_y)/area), B12_u0 = float_to_fixed(u0*bmp.width()*(v2_x - v1_x)/area); // w0
    fixed A20_u1 = float_to_fixed(u1*bmp.width()*(v2_y - v0_y)/area), B20_u1 = float_to_fixed(u1*bmp.width()*(v0_x - v2_x)/area); // w1

    fixed A01_v2 = float_to_fixed(v2*bmp.height()*(v0_y - v1_y)/area), B01_v2 = float_to_fixed(v2*bmp.height()*(v1_x - v0_x)/area); //w2
    fixed A12_v0 = float_to_fixed(v0*bmp.height()*(v1_y - v2_y)/area), B12_v0 = float_to_fixed(v0*bmp.height()*(v2_x - v1_x)/area); // w0
    fixed A20_v1 = float_to_fixed(v1*bmp.height()*(v2_y - v0_y)/area), B20_v1 = float_to_fixed(v1*bmp.height()*(v0_x - v2_x)/area); // w1

    fixed A01 = v0_y - v1_y, B01 = v1_x - v0_x; //w2
    fixed A12 = v1_y - v2_y, B12 = v2_x - v1_x; // w0
    fixed A20 = v2_y - v0_y, B20 = v0_x - v2_x; // w1

    // Barycentric coordinates at minX/minY corner
    vec2_32i p = { minX, minY };

    // overflow safety safe_bits>=(p-2)/2, i.e 15 bits (0..32,768) for 32 bits integers.
    // https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
    // for 16 bits computer this is safe for 7 bits input [0..127] - not good
    fixed w0_row_u = float_to_fixed(float(u0*bmp.width()*orient2d({v1_x, v1_y}, {v2_x, v2_y}, p))/area);
    fixed w1_row_u = float_to_fixed(float(u1*bmp.width()*orient2d({v2_x, v2_y}, {v0_x, v0_y}, p))/area);
    fixed w2_row_u = float_to_fixed(float(u2*bmp.width()*orient2d({v0_x, v0_y}, {v1_x, v1_y}, p))/area);

    fixed w0_row_v = float_to_fixed(float(v0*bmp.height()*orient2d({v1_x, v1_y}, {v2_x, v2_y}, p))/area);
    fixed w1_row_v = float_to_fixed(float(v1*bmp.height()*orient2d({v2_x, v2_y}, {v0_x, v0_y}, p))/area);
    fixed w2_row_v = float_to_fixed(float(v2*bmp.height()*orient2d({v0_x, v0_y}, {v1_x, v1_y}, p))/area);

    int w0_row = orient2d({v1_x, v1_y}, {v2_x, v2_y}, p);
    int w1_row = orient2d({v2_x, v2_y}, {v0_x, v0_y}, p);
    int w2_row = orient2d({v0_x, v0_y}, {v1_x, v1_y}, p);

//    fixed w0_row_u = fixed_mul_int(u0_F ,orient2d({v1_x, v1_y}, {v2_x, v2_y}, p));
//    fixed w1_row_u = fixed_mul_int(u1_F ,orient2d({v2_x, v2_y}, {v0_x, v0_y}, p));
//    fixed w2_row_u = fixed_mul_int(u2_F ,orient2d({v0_x, v0_y}, {v1_x, v1_y}, p));
//
//    fixed w0_row_v = fixed_mul_int(v0_F ,orient2d({v1_x, v1_y}, {v2_x, v2_y}, p));
//    fixed w1_row_v = fixed_mul_int(v1_F ,orient2d({v2_x, v2_y}, {v0_x, v0_y}, p));
//    fixed w2_row_v = fixed_mul_int(v2_F ,orient2d({v0_x, v0_y}, {v1_x, v1_y}, p));
//

    int index;


bool pik = false;

//    fixed w0u = fixed_mul_fixed()

    index = p.y*_width;

    for (p.y = minY; p.y <= maxY; p.y++) {

        // Barycentric coordinates at start of row
        fixed w0_u = w0_row_u;
        fixed w1_u = w1_row_u;
        fixed w2_u = w2_row_u;

        fixed w0_v = w0_row_v;
        fixed w1_v = w1_row_v;
        fixed w2_v = w2_row_v;

        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        int v_i = bmp_width * fixed_to_int((w0_v + w1_v + w2_v));//(int)(v * (float)bmp.height());

        for (p.x = minX; p.x <= maxX; p.x++) {
//            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
            // same as (w0 >= 0 && w1 >= 0 && w2 >= 0), but use only MSB,
            // this turns three conditionals into one !!!
//            if ((w0 | w1 | w2) >= 0) {
            if ((w0 | w1 | w2) >= 0) {

                int u_i = fixed_to_int((w0_u + w1_u + w2_u));//(int)(u * (float)bmp.width());
//                int v_i = bmp_width * fixed_to_int((w0_v + w1_v + w2_v));//(int)(u * (float)bmp.width());
                int index_bmp = (v_i + u_i);

                color_f_t col_bmp;
                bmp.decode(index_bmp, col_bmp);
                blendColor(col_bmp, index + p.x);

                // this is faster if we don't use blending
//                drawPixel(bmp.pixelAt(index_bmp), index + p.x);
            }

            // One step to the right
            w0_u += A12_u0;
            w1_u += A20_u1;
            w2_u += A01_u2;

        w0_v += A12_v0;
        w1_v += A20_v1;
        w2_v += A01_v2;

            w0 += A12;
            w1 += A20;
            w2 += A01;

        }

//        w0_v += A12_v0;
//        w1_v += A20_v1;
//        w2_v += A01_v2;

        // One row step
        w0_row_u += B12_u0;
        w1_row_u += B20_u1;
        w2_row_u += B01_u2;

        w0_row_v += B12_v0;
        w1_row_v += B20_v1;
        w2_row_v += B01_v2;

        w0_row += B12;
        w1_row += B20;
        w2_row += B01;

        index += _width;
    }

}



//template<typename P, typename CODER>
//template<typename P2, typename CODER2>
//void
//Canvas<P, CODER>::drawTriangle(Bitmap<P2, CODER2> & bmp,
//                               int v0_x, int v0_y, float u0, float v0,
//                               int v1_x, int v1_y, float u1, float v1,
//                               int v2_x, int v2_y, float u2, float v2) {
//
////    float area = orient2d({v0_x, v0_y}, {v1_x, v1_y}, {v2_x, v2_y});
//    float area = orient2d({v0_x, v0_y}, {v1_x, v1_y}, {v2_x, v2_y});
//
//    // bounding box
//    int minX = std::min({v0_x, v1_x, v2_x});
//    int minY = std::min({v0_y, v1_y, v2_y});
//    int maxX = std::max({v0_x, v1_x, v2_x});
//    int maxY = std::max({v0_y, v1_y, v2_y});
//
//    // Triangle setup
//    int A01 = v0_y - v1_y, B01 = v1_x - v0_x;
//    int A12 = v1_y - v2_y, B12 = v2_x - v1_x;
//    int A20 = v2_y - v0_y, B20 = v0_x - v2_x;
//
//    // Barycentric coordinates at minX/minY corner
//    vec2_32i p = { minX, minY };
//
//    // overflow safety safe_bits>=(p-2)/2, i.e 15 bits (0..32,768) for 32 bits integers.
//    // https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
//    // for 16 bits computer this is safe for 7 bits input [0..127] - not good
//    int w0_row = orient2d({v1_x, v1_y}, {v2_x, v2_y}, p);
//    int w1_row = orient2d({v2_x, v2_y}, {v0_x, v0_y}, p);
//    int w2_row = orient2d({v0_x, v0_y}, {v1_x, v1_y}, p);
//    int index;
//
//    fixed one_over_area = fixed_one_over_int(area);
//    fixed u0_F = float_to_fixed(u0);
//    fixed u1_F = float_to_fixed(u1);
//    fixed u2_F = float_to_fixed(u1);
//
////    fixed w0u = fixed_mul_fixed()
//
//    index = p.y*_width;
//
//    for (p.y = minY; p.y <= maxY; p.y++) {
//
//        // Barycentric coordinates at start of row
//        fixed w0 = w0_row;
//        fixed w1 = w1_row;
//        fixed w2 = w2_row;
//
//
//        for (p.x = minX; p.x <= maxX; p.x++) {
////            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
//            // same as (w0 >= 0 && w1 >= 0 && w2 >= 0), but use only MSB,
//            // this turns three conditionals into one !!!
//            if ((w0 | w1 | w2) >= 0) {
//
////                float w0_n = w0/area;
////                float w1_n = w1/area;
////                float w2_n = w2/area;
////                float u = w0_n * u0 + w1_n * u1 + w2_n * u2;
////                float v = w0_n * v0 + w1_n * v1 + w2_n * v2;
////                int u_i = (int)(u * (float)bmp.width());
////                int v_i = (int)(v * (float)bmp.height());
////                int index_bmp = (v_i * bmp.width() + u_i);
//
////                fixed w0_n = fixed_mul_int(w0, one_over_area);
////                fixed w1_n = fixed_mul_int(w1, one_over_area);
////                fixed w2_n = fixed_mul_int(w2, one_over_area);
////                float u = (w0_n * u0 + w1_n * u1 + w2_n * u2);
////                float v = (w0_n * v0 + w1_n * v1 + w2_n * v2);
//
//                float u = (w0 * u0 + w1 * u1 + w2 * u2)/area;
//                float v = (w0 * v0 + w1 * v1 + w2 * v2)/area;
//                int u_i = (int)(u * (float)bmp.width());
//                int v_i = (int)(v * (float)bmp.height());
//                int index_bmp = (v_i * bmp.width() + u_i);
//
//                color_f_t col_bmp;
//                bmp.decode(index_bmp, col_bmp);
//                blendColor(col_bmp, index + p.x);
//
//                // this is faster if we don't use blending
////                P output{};
//
////                drawPixel(bmp.pixelAt(index_bmp), p.x, p.y);
//            }
//
//            // One step to the right
//            w0 += A12;
//            w1 += A20;
//            w2 += A01;
//
//        }
//
//        // One row step
//        w0_row += B12;
//        w1_row += B20;
//        w2_row += B01;
//        index += _width;
//    }
//
//}



//
//template<typename P, typename CODER>
//template<typename P2, typename CODER2>
//void
//Canvas<P, CODER>::drawTriangle(Bitmap<P2, CODER2> & bmp,
//                                int v0_x, int v0_y, float u0, float v0,
//                                int v1_x, int v1_y, float u1, float v1,
//                                int v2_x, int v2_y, float u2, float v2) {
//    color_f_t col_bmp;
//
//    float area = edgeFunction(v0_x, v0_y, v1_x, v1_y, v2_x, v2_y);
//
//    // bounding box
//    int x1 = std::min({v0_x, v1_x, v2_x});
//    int y1 = std::min({v0_y, v1_y, v2_y});
//    int x2 = std::max({v0_x, v1_x, v2_x});
//    int y2 = std::max({v0_y, v1_y, v2_y});
//
//    for (uint32_t y = y1; y < y2; ++y) {
//        for (uint32_t x = x1; x < x2; ++x) {
//            vec3_f p = {x + 0.5f, y + 0.5f, 0};
//
//            float w0 = edgeFunction<float>(v1_x, v1_y, v2_x, v2_y, p.x, p.y);
//            float w1 = edgeFunction<float>(v2_x, v2_y, v0_x, v0_y, p.x, p.y);
//            float w2 = edgeFunction<float>(v0_x, v0_y, v1_x, v1_y, p.x, p.y);
//
//            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
//                w0 /= area;
//                w1 /= area;
//                w2 /= area;
//
//                float u = w0 * u0 + w1 * u1 + w2 * u2;
//                float v = w0 * v0 + w1 * v1 + w2 * v2;
//                int u_i = (int)(u * (float)bmp.width());
//                int v_i = (int)(v * (float)bmp.height());
//
//                int index_bmp = (v_i * bmp.width() + u_i);
//
//
//                // decode the bitmap
//                bmp.decode(index_bmp, col_bmp);
//                // re-encode for a different canvas
//                blendColor(col_bmp, x, y);
//
//                // todo: also connect to compositing
//                // normalize to recode it to our format later
//                // get rid of this, we should only care to read
//                // rgba values and should not care about the bitmap
//                // internal pixels type P. this way we can use different
//                // bitmap types and recode them for our purposes in case
//                // their internal type is different
////                P d = bmp.readAt(index_bmp);
////                drawPixel(d, x, y);
//
//
//            }
//
//        }
//
//    }
//
//}


template<typename P, typename CODER>
void Canvas<P, CODER>::drawQuad(const color_f_t & color,
                                const int left, const int top,
                                const int right, const int bottom) {
    int index = top * _width;
    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < right; ++x) {
            blendColor(color, index + x);
        }

        index += _width;
    }

}

///*

template<typename P, typename CODER>
template<typename P2, typename CODER2>
void Canvas<P, CODER>::drawQuad(Bitmap<P2, CODER2> &bmp,
                                 const int left, const int top,
                                 const int right, const int bottom) {
    color_f_t col_bmp{};
    P converted{};

    int bmp_width = (bmp.width());
    int bmp_height = (bmp.height());

    fixed du = fixed_div_int(int_to_fixed(bmp_width), right-left);
    fixed dv = fixed_div_int(int_to_fixed(bmp_height), bottom-top);
    fixed u = -du, v = -dv;

    int u_i=0, v_i=0, v_f_i=0,v_f_i_last=0;
    int index_bmp, index;

    index = top * _width;

    for (int y = top; y < bottom; y++) {
        v += dv;
        // v_i with multiplication
//        v_i = fixed_to_int(v)*(bmp_width);

//todo: this might be wrong
        /// v_i without multiplication
        v_f_i = fixed_to_int(v);

        // things have changed
        if(v_f_i>v_f_i_last) {
            v_i += bmp_width;
        }

        v_f_i_last = v_f_i;
        ///


        for (int x = left; x < right; x++) {
            u += du;
            u_i = fixed_to_int(u);
            index_bmp = (v_i) + u_i;


            // decode the bitmap
            bmp.decode(index_bmp, col_bmp);
            // re-encode for a different canvas
            blendColor(col_bmp, index + x);

            //
            // TODO:: optimization note,
            // if we copy from same bitmap formats without blending/compositing, than it is
            // 10% of the running with composting etc... so use it for optimization.
//             converted = bmp.pixelAt(index_bmp);
//             drawPixel(converted, index + x);


        }
        u = -du;
        index += _width;


    }

}

#pragma clang diagnostic pop