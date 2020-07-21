#pragma once

#include "common_types.h"

struct RenderedChar {
    int32_t symbol;
    int offsetX, offsetY, w, h, advance;
    Img *img;
    std::map<int32_t,int> kerning;
    RenderedChar(uint symbol, int x, int y, int w, int h, int a, Img* img) :
            symbol(symbol), offsetX(x), offsetY(y), w{w}, h{h}, advance(a), img(img) {}
};

struct RenderedMetrics {
    int ascender, descender, height;
};

struct FontRendererResult {
    std::map<int32_t , RenderedChar> chars;
    RenderedMetrics metrics;
};
