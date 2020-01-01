
#include <iostream>
#include <chrono>
#include <SDL2/SDL.h>
#include <microgl/color.h>
#include <microgl/Canvas.h>
#include <microgl/matrix_4x4.h>
#include <microgl/Q.h>
#include <microgl/camera.h>
#include <microgl/dynamic_array.h>

#define TEST_ITERATIONS 1
#define W 640*1
#define H 480*1

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;
using Canvas24Bit_Packed32 = Canvas<uint32_t, RGB888_PACKED_32>;

Canvas24Bit_Packed32 * canvas;

float t = 0.0f;

void loop();
void init_sdl(int width, int height);
//template <typename number_transform, typename number_raster>
//void render_template();

template <typename number>
using arr = dynamic_array<vec3<number>>;

template <typename number>
arr<number> cube() {

    return {
            //down
            {-1, -1, -1}, // left-bottom
            {-1, -1, 1}, // left-top
            {1, -1, 1}, // right-top
            {1, -1, -1}, // right-bottom
            //up
            {-1, 1, -1}, // left-bottom
            {-1, 1, 1}, // left-top
            {1, 1, 1}, // right-top
            {1, 1, -1}, // right-bottom
    };

}

float z = 0;

template <typename number_coords>
void render_template(const arr<number_coords> & vertices) {
    using vertex = vec3<number_coords>;
    using camera = microgl::camera<number_coords>;
    using mat4 = matrix_4x4<number_coords>;
    using math = microgl::math;

    z+=0.121;

    int canvas_width = canvas->width();
    int canvas_height = canvas->height();
    number_coords fov_horizontal = math::deg_to_rad(60);
//    number_coords aspect_ratio = microgl::math::deg_to_rad(90);

    // model is in z range: [-1, -500]
    mat4 model = mat4::transform({0, math::deg_to_rad(z), 0}, {0,0,-300+25}, {25,25,25});
//    mat4 view = camera::lookAt({0, 0, 250}, {0,0, 0}, {0,1,0});
    mat4 view = camera::angleAt({0, 0, 0}, math::deg_to_rad(0),
            math::deg_to_rad(0), math::deg_to_rad(0));
    mat4 projection = camera::perspective(fov_horizontal, canvas_width, canvas_height, 1, 100);
//    mat4 perspective = camera::orthographic(-canvas_width, canvas_width, -canvas_height, canvas_height, 1, 100.0);
    mat4 mvp = projection * (view * model);

    canvas->clear(WHITE);

    for (unsigned ix = 0; ix < vertices.size(); ++ix) {
        vertex vv = mat4(view * model) * vertices[ix];
        vertex ndc_projected = projection * vv;//vertices[ix];
        vertex raster;
        // convert to raster space
        raster.x = ((ndc_projected.x + number_coords(1))*canvas_width)/2;
        raster.y = canvas_height - (((ndc_projected.y + number_coords(1))*canvas_height)/2); // invert y for raster space
        raster.z = (ndc_projected.z + number_coords(1))/number_coords(2);

        bool inside = (raster.x >= 0) &&  (raster.x < canvas_width) &&
                (raster.y >= 0) &&  (raster.y < canvas_height) && (math::abs_(raster.z) <= 1);
//        if(!inside)
//            continue;

        std::cout << raster.x << ", " << raster.y << ", " << raster.z << " - " << z <<std::endl;
        auto color = RED;
        if(ix>=7) color=BLUE;
        canvas->drawCircle(color, raster.x, raster.y, number_coords(4), 255);
    }

}

void render() {

    render_template<float>(cube<float>());
}

int main() {
    init_sdl(W, H);
    loop();
}

void init_sdl(int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL2 Pixel Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, width, height);
    canvas = new Canvas24Bit_Packed32(width, height, new RGB888_PACKED_32());
}

int render_test(int N) {
    auto ms = std::chrono::milliseconds(1);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        render();
    }

    auto end = std::chrono::high_resolution_clock::now();

    return (end-start)/(ms*N);
}

void loop() {
    bool quit = false;
    SDL_Event event;

    // 100 Quads
//    int ms = render_test(TEST_ITERATIONS);
//    std::cout << ms << std::endl;

    while (!quit)
    {
        SDL_PollEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYUP:
                if( event.key.keysym.sym == SDLK_ESCAPE )
                    quit = true;
                break;
        }

        render();

        SDL_UpdateTexture(texture, nullptr, canvas->pixels(), canvas->width() * canvas->sizeofPixel());
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}