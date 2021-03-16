#include "src/Resources.h"
#include "src/example.h"
#include <microgl/canvas.h>
#include <microgl/pixel_coders/RGB888_PACKED_32.h>
#include <microgl/pixel_coders/RGB888_ARRAY.h>
#include <microgl/samplers/texture.h>
#include <microgl/samplers/flat_color.h>

#define TEST_ITERATIONS 1
#define W 640*1
#define H 640*1

template <typename number>
vec3<number>* bi_cubic_1(){

    return new vec3<number>[4*4] {
            {1.0f, 0.0f},
            {170.66f, 0.0f},
            {341.333f, 0.0f},
            {512.0f, 0.0f},

            {1.0f,       170.66f},
            {293.44f,    162.78f},
            {746.68f,    144.65f},
            {512.0f,     170.66f},

            {1.0f,       341.33f},
            {383.12f,    327.69f},
            {1042.79f,   296.31f},
            {512.0f,     341.33f},

            {1.0f,       512.0f},
            {170.66f,    512.0f},
            {341.333f,   512.0f},
            {512.0f,     512.0f}
    };
}

int main() {
    using number = float;
//    using number = Q<16>;

    using Canvas24= canvas<bitmap<coder::RGB888_PACKED_32>>;
    using Texture24= sampling::texture<bitmap<coder::RGB888_ARRAY>, sampling::texture_filter::NearestNeighboor>;
    sampling::flat_color<> color_grey{{222,222,222,255}};
    Resources resources{};

    Canvas24 canvas(W, H);

    auto img_2 = resources.loadImageFromCompressedPath("images/uv_512.png");
    Texture24 tex_uv{new bitmap<coder::RGB888_ARRAY>(img_2.data, img_2.width, img_2.height)};
    constexpr int samples = 20;
    auto test_bezier = [&](vec3<number>* mesh, unsigned U, unsigned V) {
        canvas.clear({255,255,255,255});
        canvas.drawBezierPatch<blendmode::Normal, porterduff::None<>, false, false, number, number>(
//            color_grey,
                tex_uv,
                matrix_3x3<number>::identity(),
                mesh, U, V, samples, samples,
                0,1,1,0,
                255);
        delete [] mesh;
    };

    auto render = [&]() -> void {

        test_bezier(bi_cubic_1<number>(), 4, 4);
    };

    example_run(&canvas,
                render);
}
