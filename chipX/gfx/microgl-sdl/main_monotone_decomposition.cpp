#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#include <iostream>
#include <chrono>
#include "src/Resources.h"
#include <SDL2/SDL.h>
#include <microgl/FrameBuffer.h>
#include <microgl/Canvas.h>
#include <microgl/Types.h>
#include <microgl/PixelCoder.h>
#include <microgl/MonotoneDecomposition.h>
#include <microgl/AVLTree.h>

#define TEST_ITERATIONS 1
#define W 640*1
#define H 480*1

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;

typedef Canvas<uint32_t, RGB888_PACKED_32> Canvas24Bit_Packed32;

Canvas24Bit_Packed32 * canvas;

Resources resources{};

color_f_t RED{1.0,0.0,0.0, 1.0};
color_f_t YELLOW{1.0,1.0,0.0, 1.0};
color_f_t WHITE{1.0,1.0,1.0, 1.0};
color_f_t GREEN{0.0,1.0,0.0, 1.0};
color_f_t BLUE{0.0,0.0,1.0, 1.0};
color_f_t BLACK{0.0,0.0,0.0, 1.0};

void loop();
void init_sdl(int width, int height);

using namespace tessellation;
using namespace ds;

void render_star();

float t = 20;

void render() {
    render_star();
}



void render_star() {

    vec2_32i p0 = {100,100};
    vec2_32i p1 = {300, 100};
    vec2_32i p2 = {120, 300};
    vec2_32i p3 = {200, 20};
    vec2_32i p4 = {280,300};
//    vec2_32i p5 = {100, 100};

    std::vector<vec2_32i> polygon = {p0, p1, p2, p3, p4, p0};//, p5};

    canvas->clear(WHITE);


    monotone::MonotoneDecomposition<int32_t> monotoneDecomposition(true);

    uint8_t precision = 0;
//    auto & I = monotoneDecomposition.compute(polygon.data(), polygon.size(), precision);

//    for (auto & inter : I) {
//        canvas->drawCircle<blendmode::Normal, porterduff::SourceOverOnOpaque, true>(
//                RED, inter.x, inter.y, 8<<precision, precision, 80);
//    }

    canvas->drawLinePath(BLACK, polygon.data(), polygon.size());

}


int main() {
    init_sdl(W, H);
    loop();
}

void init_sdl(int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL2 Pixel Drawing", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
            SDL_TEXTUREACCESS_STATIC, width, height);

    canvas = new Canvas24Bit_Packed32(width, height, new RGB888_PACKED_32());

    resources.init();
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
    int ms = render_test(TEST_ITERATIONS);
    cout << ms << endl;

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
//
//        render();

        SDL_UpdateTexture(texture, nullptr, canvas->pixels(),
                canvas->width() * canvas->sizeofPixel());
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

#pragma clang diagnostic pop